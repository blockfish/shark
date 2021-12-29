const rules = require('../rules');

const INITIAL = null;

function reducer(edit, board, palette, action) {
    let {selected, fillRow} = palette;

    switch (action.type) {
    case 'edit/drag/start':
        {
            let {x, y} = action.payload;
            let fill = selected;
            let fillCells = drawingCells(fill, x, y, fillRow);
            let newPlayfield = board.playfield.setCells(fillCells);
            // start erasing if drawing would have no effect, such as clicing on a
            // cell with the same color as the selected color.
            if (newPlayfield === board.playfield) {
                fill = null;
                fillCells = drawingCells(fill, x, y, fillRow);
                newPlayfield = board.playfield.setCells(fillCells);
            }
            board = commitEdit(edit, board);
            edit = {type: 'draw', fill, playfield: newPlayfield};
        }
        break;

    case 'edit/drag/drag':
        if (edit && edit.type === 'draw') {
            let {x, y} = action.payload;
            let fillCells = drawingCells(edit.fill, x, y, fillRow);
            let newPlayfield = edit.playfield.setCells(fillCells);
            if (newPlayfield !== edit.playfield) {
                edit = {...edit, playfield: newPlayfield};
            }
        }
        break;

    case 'edit/drag/end':
        if (edit && edit.type === 'draw') {
            board = commitEdit(edit, board);
            edit = null;
        }
        break;

    case 'edit/clickHold':
        board = commitEdit(edit, board);
        edit = {type: 'hold', hold: null};
        break;

    case 'edit/clickQueue':
        board = commitEdit(edit, board);
        edit = {type: 'queue', queue: []};
        break;

    case 'edit/enterPiece':
        switch (edit && edit.type) {
        case 'hold':
            board = commitEdit({...edit, hold: action.payload}, board);
            edit = null;
            break;

        case 'queue':
            // TODO(iitalics): allow for more pieces than ruleset previews
            edit = {...edit, queue: [...edit.queue, action.payload]};
            if (edit.queue.length >= rules.previews) {
                board = commitEdit(edit, board);
                edit = null;
            }
            break;
        }
        break;

    case 'edit/cancel':
        edit = null;
        break;

    case 'edit/otherKey':
        switch (edit && edit.type) {
        case 'hold':
        case 'queue':
            board = commitEdit(edit, board);
            edit = null;
            break;
        }
        break;
    }

    return [edit, board];
}

function commitEdit(edit, board) {
    switch (edit && edit.type) {
    case 'draw': return {...board, playfield: edit.playfield};
    case 'hold': return {...board, hold: edit.hold};
    case 'queue': return {...board, queue: edit.queue};
    default: return board;
    }
}
        
function* drawingCells(type, x, y, fillRow) {
    if (fillRow) {
        for (let i = 0; i < rules.cols; i++) {
            yield { type: (x === i) ? null : type, x: i, y };
        }
    } else {
        yield { type, x, y };
    }
}

let clickHold = dispatch => () => dispatch({type: 'edit/clickHold'});
let clickQueue = dispatch => () => dispatch({type: 'edit/clickQueue'});
let cancel = dispatch => () => dispatch({type: 'edit/cancel'});
let enterPiece = dispatch => type => dispatch({type: 'edit/enterPiece', payload: type});
let otherKey = dispatch => key => dispatch({type: 'edit/otherKey', payload: key});
let drag = dispatch => (type, x, y) => dispatch({type: `edit/drag/${type}`, payload: {x, y}});

module.exports = {
    INITIAL,
    reducer,
    clickHold,
    clickQueue,
    cancel,
    enterPiece,
    otherKey,
    drag,
};

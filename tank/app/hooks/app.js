const { Matrix } = require('../matrix');
const rules = require('../rules');

function init() {
    let bag = Object.keys(rules.shapes);
    let initQueue = new Array(5);
    for (let i = 0; i < 5; i++) {
        let j = Math.floor(Math.random() * bag.length);
        initQueue[i] = bag.splice(j, 1)[0];
    }

    let initPlayfield = Matrix.EMPTY;

    return {
        board: {
            playfield: initPlayfield,
            queue: initQueue,
            hold: null,
        },
        palette: {
            selected: 'G',
            fillRow: true,
        },
        autoplay: {
            playing: false,
            garbage: false,
        },
        edit: null,
    };
}

function* drawingCells(fillType, fillX, fillY, fillRow) {
    if (fillRow) {
        for (let x = 0; x < rules.cols; x++) {
            yield { x: x, y: fillY, type: (x === fillX) ? null : fillType };
        }
    } else {
        yield { x: fillX, y: fillY, type: fillType };
    }
}

function reducer(state, action) {
    let board = state.board;
    let palette = state.palette;
    let autoplay = state.autoplay;
    let edit = state.edit;

    function commitCurrentEdit() {
        switch (edit && edit.type) {
        case 'draw': board = {...board, playfield: edit.playfield}; break;
        case 'enter-hold': board = {...board, hold: edit.hold}; break;
        case 'enter-queue': board = {...board, queue: edit.queue}; break;
        }
        edit = null;
    }

    switch (action.type) {
    case 'palette:select-type':
        palette = {...palette, selected: action.select};
        break;
    case 'palette:toggle-fill-row':
        palette = {...palette, fillRow: !palette.fillRow};
        break;

    case 'edit:drag:start':
        {
            let fillType = palette.selected;
            let newPlayfield = board.playfield.setCells(drawingCells(
                fillType,
                action.x,
                action.y,
                palette.fillRow,
            ));
            // start erasing if drawing would have no effect, such as clicing on a
            // cell with the same color as the selected color.
            if (newPlayfield === board.playfield) {
                fillType = null;
                newPlayfield = board.playfield.setCells(drawingCells(
                    null,
                    action.x,
                    action.y,
                    palette.fillRow,
                ));
            }
            commitCurrentEdit();
            edit = {
                type: 'draw',
                fill: fillType,
                playfield: newPlayfield,
            };
            break;
        }
        
    case 'edit:drag:drag':
        if (edit && edit.type === 'draw') {
            let newPlayfield = edit.playfield.setCells(drawingCells(
                edit.fill,
                action.x,
                action.y,
                palette.fillRow,
            ));
            if (newPlayfield === edit.playfield) {
                break;
            }
            edit = {...edit, playfield: newPlayfield};
        }
        break;

    case 'edit:drag:end':
        if (edit && edit.type === 'draw') {
            commitCurrentEdit();
        }
        break;

    case 'edit:click-hold':
        commitCurrentEdit();
        edit = {type: 'enter-hold', hold: null};
        break;
    case 'edit:click-queue':
        commitCurrentEdit();
        edit = {type: 'enter-queue', queue: []};
        break;

    case 'edit:enter-piece':
        switch (edit && edit.type) {
        case 'enter-hold':
            edit = {...edit, hold: action.piece};
            commitCurrentEdit();
            break;
        case 'enter-queue':
            edit = {...edit, queue: [...edit.queue, action.piece]};
            // TODO(iitalics): allow for more pieces than ruleset previews
            if (edit.queue.length >= rules.previews) {
                commitCurrentEdit();
            }
            break;
        }
        break;

    case 'edit:cancel':
        edit = null;
        break;

    case 'play:toggle-garbage':
        commitCurrentEdit();
        autoplay = {...autoplay, garbage: !autoplay.garbage};
        // TODO(iitalics): if true then generate new garbage/queue immediately
        break;
    case 'play:toggle-play':
        commitCurrentEdit();
        autoplay = {...autoplay, playing: !autoplay.playing};
        break;
    case 'play:step-forward':
        commitCurrentEdit();
        break;

    case 'undo':
        break;
    case 'redo':
        break;

    case 'other-key':
        switch (edit && edit.type) {
        case 'enter-hold':
        case 'enter-queue':
            commitCurrentEdit();
            break;
        }
        break;
    }

    return {
        board,
        palette,
        autoplay,
        edit,
    };
}

function useApp() {
    let [state, dispatch] = React.useReducer(reducer, null, init);
    return [state.board, state.palette, state.autoplay, state.edit, dispatch];
}

module.exports = { useApp };

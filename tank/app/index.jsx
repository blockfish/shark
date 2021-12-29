const rules = require('./rules');
const { Matrix } = require('./matrix');
const { useKeyEvents } = require('./hooks/keys');
const { SideBar } = require('./components/side');
const { Board } = require('./components/board');
const { BottomBar } = require('./components/bottom');
const Palette = require('./reducers/palette');
const Edit = require('./reducers/edit');
const Play = require('./reducers/play');

function initBoard() {
    let playfield = Matrix.EMPTY;
    let bag = Object.keys(rules.shapes);
    let queue = new Array(5);
    for (let i = 0; i < 5; i++) {
        let j = Math.floor(Math.random() * bag.length);
        queue[i] = bag.splice(j, 1)[0];
    }
    let hold = null;
    return {playfield, queue, hold};
}

function init() {
    let board = initBoard();
    let palette = Palette.INITIAL;
    let edit = Edit.INITIAL;
    let play = Play.INITIAL;
    let bot = {status: 'disabled', move: null};
    return {board, palette, edit, play, bot};
}

function reducer(state, action) {
    let {board, palette, edit, play, bot} = state;
    palette = Palette.reducer(palette, action);
    [edit, board] = Edit.reducer(edit, board, palette, action);
    play = Play.reducer(play, action);
    return {board, palette, edit, play, bot};
}

let Main = () => {
    let [state, dispatch] = React.useReducer(reducer, undefined, init);

    let [
        onEnterPiece,
        onCancel,
        onOtherKey,
        onClickHold,
        onClickQueue,
        onDrag,
        onToggleFillRow,
        onSelectType,
        onToggleGarbage,
        onToggleAuto,
        onStep,
    ] = React.useMemo(() => [
        Edit.enterPiece(dispatch),
        Edit.cancel(dispatch),
        Edit.otherKey(dispatch),
        Edit.clickHold(dispatch),
        Edit.clickQueue(dispatch),
        Edit.drag(dispatch),
        Palette.toggleFillRow(dispatch),
        Palette.selectType(dispatch),
        Play.toggleGarbage(dispatch),
        Play.toggleAuto(dispatch),
        Play.step(dispatch),
    ], [dispatch]);

    // FIXME(iitalics): useBot

    useKeyEvents(window, onEnterPiece, onCancel, onOtherKey);

    return (
        <main className="grid hmc vm4">
            <SideBar
                palette={state.palette}
                onSelectType={onSelectType}
                onToggleFillRow={onToggleFillRow} />
            <Board
                board={state.board}
                edit={state.edit}
                bot={state.bot}
                onClickHold={onClickHold}
                onClickQueue={onClickQueue}
                onDrag={onDrag} />
            <BottomBar
                play={state.play}
                bot={state.bot}
                onToggleGarbage={onToggleGarbage}
                onToggleAuto={onToggleAuto}
                onStep={onStep} />
        </main>
    );
};

ReactDOM.render(<Main />, document.getElementById('root'));

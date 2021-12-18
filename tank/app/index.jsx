const { useApp } = require('./hooks/app');
const { useKeyEvents } = require('./hooks/keys');
const { useBot } = require('./hooks/bot');
const { SideBar } = require('./components/side');
const { Board } = require('./components/board');
const { BottomBar } = require('./components/bottom');

let Main = () => {
    let [board, palette, autoplay, edit, dispatch] = useApp();
    let bot = useBot(board, edit, dispatch);
    useKeyEvents(window, dispatch);
    return (
        <main className="grid hmc vm4">
            <SideBar
                palette={palette}
                dispatch={dispatch} />
            <Board
                board={board}
                edit={edit}
                dispatch={dispatch} />
            <BottomBar
                autoplay={autoplay}
                bot={bot}
                dispatch={dispatch} />
        </main>
    );
};

ReactDOM.render(<Main />, document.getElementById('root'));

const { Bot } = require('../tbp');

window['TBP'] = require('../tbp');
window['MATRIX'] = require('../matrix');

function useBot(board, edit, dispatch) {
    let botRef = React.useRef();

    let [ready, setReady] = React.useState(null);
    let [error, setError] = React.useState(null);
    React.useEffect(() => {
        let bot = (botRef.current ||= new Bot());
        bot.onReady = setReady;
        bot.onError = setError;
    }, [botRef]);

    let [task, setTask] = React.useState(null);
    React.useEffect(() => {
        if (ready) {
            let bot = botRef.current;
            if (edit) {
                if (task !== null) {
                    task.cancel();
                    setTask(null);
                }
            } else if (task === null || task.board !== board) {
                setTask(bot.start(board));
            }
        }
    }, [ready, edit, task, board]);

    let [move, setMove] = React.useState(null);
    React.useEffect(() => {
        if (task) {
            task.onChange = setMove;
        } else {
            setMove(null);
        }
    }, [task]);

    let status = (
        error ? `error: ${error}`
            : !ready ? `loading`
            : !task ? `paused`
            : `thinking`
    );

    return React.useMemo(() => ({ status, move }), [status, move]);
}

module.exports = { useBot };

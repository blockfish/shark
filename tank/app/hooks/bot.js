function useBotWebWorker(board, edit, onBotError, onBotCalculating, onBotSuggestion) {
    let worker = React.useRef(null);
    function send(msg) { worker.current.postMessage(msg); }
    React.useEffect(() => {
        worker.current = new Worker('/webworker.js');
        return () => worker.current.close();
    }, [worker]);

    let [busy, setBusy] = React.useState(true);
    let [calculating, setCalculating] = React.useState(null);
    let [suggest, setSuggest] = React.useState(false);

    function onMessage(msg) {
        switch (msg.type) {
        case 'error':
            setBusy(true);
            onBotError(msg.reason);
            break;
        case 'info':
            send(Msg.rules);
            console.log(msg);
            break;
        case 'ready':
            setBusy(false);
            break;
        case 'suggestion':
            setBusy(false);
            onBotSuggestion(msg.moves.map(m => m.location));
            break;
        default:
            console.warn('ignored message from bot:', msg);
            break;
        }
    }
    React.useEffect(() => {
        worker.current.onmessage = ev => onMessage(ev.data);
        return () => worker.current.onmessage = undefined;
    }, [worker, onBotError, onBotSuggestion]);

    let target = edit ? null : board;
    React.useEffect(() => onBotCalculating(!!target), [!!target, onBotCalculating]);

    React.useEffect(() => {
        if (busy) {
            return;
        }
        if (calculating !== target) {
            if (target) {
                send(Msg.start(target));
            } else {
                send(Msg.stop);
            }
            setCalculating(target);
        } else if (suggest) {
            send(Msg.suggest);
            setSuggest(false);
        }
    }, [busy, calculating, target, suggest]);

    let poll = React.useRef(null);
    React.useEffect(() => {
        if (calculating) {
            poll.current = setInterval(() => setSuggest(true), 200);
        }
        return () => {
            if (poll.current) {
                clearInterval(poll.current);
                poll.current = null;
            }
        };
    }, [calculating]);

}

const Msg = {
    rules: {type: 'rules'},
    stop: {type: 'stop'},
    suggest: {type: 'suggest'},
    _start: {type: 'start', back_to_back: false, combo: 0},
    start({playfield, queue, hold}) {
        let board = new Array(40);
        for (let y = 0; y < 40; y++) {
            board[y] = new Array(10);
            for (let x = 0; x < 10; x++) {
                board[y][x] = null;
            }
        }
        for (let {type, x, y} of playfield.getFilledCells()) {
            board[y][x] = type;
        }
        return {...Msg._start, board, queue, hold};
    },
};

module.exports = { useBotWebWorker };

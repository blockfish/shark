const RULES = { type: 'rules' };
const SUGGEST = { type: 'suggest' };
const STOP = { type: 'stop' };

const START = { type: 'start', back_to_back: false, combo: 0 };
function startMessage({ playfield, queue, hold }) {
    let board = new Array(40);
    for (let y = 0; y < 40; y++) {
        board[y] = new Array(10);
        for (let x = 0; x < 10; x++) {
            board[y][x] = null;
        }
    }
    for (let { x, y, type } of playfield.getFilledCells()) {
        board[y][x] = type;
    }
    return { ...START, board, queue, hold };
}

class Bot {
    constructor() {
        this._worker = new Worker('/webworker.js');
        this._worker.onmessage = (ev) => {
            this._onRecv(ev.data);
        };

        this.info = null;
        this._calculating = null;
        this._suggesting = null;

        this.onReady = undefined;
        this.onError = undefined;
    }

    _send(msg) {
        this._worker.postMessage(msg);
    }

    _onRecv(msg) {
        switch (msg.type) {
        case 'info':
            this.info = {
                name: msg.name,
                version: msg.version,
                author: msg.author,
            };
            this._send(RULES);
            break;

        case 'error':
            console.error('bot error:', msg.reason);
            if (this.onError) {
                this.onError(msg.reason);
            }
            break;

        case 'ready':
            if (this.onReady) {
                this.onReady(this.info);
            }
            break;

        case 'suggestion':
            if (this._suggesting !== null) {
                let task = this._suggesting.pop();
                this._suggesting = null;
                if (task === this._calculating) {
                    task.receiveSuggestion(msg.moves);
                } else {
                    this._reallyStart(task);
                }
            }
            break;

        default:
            console.warn('ignored bot message:', msg);
            break;
        }
    }

    start(board) {
        // - TBP can only have *one* active task at a time
        // - if this task is mid-suggestion then we have to wait until the
        // suggestion is received before starting a new task
        // - if the bot is currently calculating then we should stop() it before
        // starting a new task (not required, but let's do the right thing here)
        // - UNLESS the new board is achieved by a specific placement from the currently
        // calculating board; then we should use 'play' message instead of 'stop'+'start'
        let task = new BotTask(board, this);
        if (this._calculating && this._suggesting) {
            // only start the task when the current suggestion finishes
            this._suggesting.push(task);
        } else {
            this._reallyStart(task);
        }
        return task;
    }

    _reallyStart(task) {
        if (this._calculating !== null) {
            this._send(STOP);
        }
        this._send(startMessage(task.board));
        this._calculating = task;
        this._suggesting = null;
    }

    requestSuggestion(task) {
        if (this._calculating === task && this._suggesting === null) {
            this._send(SUGGEST);
            this._suggesting = [task];
        } else {
            console.warn('BUG: bad suggestion request for task', task);
        }
    }

    stop(task) {
        if (this._calculating === task) {
            this._send(STOP);
            this._calculating = null;
        }
    }
}

class BotTask {
    constructor(board, bot) {
        this.board = board;
        this.bestMove = null;
        this.onChange = undefined;
        this._requestCount = 0;

        let ival = setInterval(() => bot.requestSuggestion(this), BotTask.REQUEST_INTERVAL);
        this._stop = () => {
            clearInterval(ival);
            bot.stop(this);
        };
    }

    cancel() {
        this._stop();
        this._stop = () => {};
    }

    receiveSuggestion(moves) {
        if (moves.length > 0) {
            this._updateBestMove(moves[0].location);
        } else {
            // TODO(iitalics): forfeit? or just didn't finish finding move?
        }

        this._requestCount++;
        if (this._requestCount >= BotTask.MAX_REQUESTS) {
            this.cancel();
        }
    }
    
    _updateBestMove(move) {
        if (this.bestMove !== null
            && move.x === this.bestMove.x && move.y === this.bestMove.y
            && move.type === this.bestMove.type
            && move.orientation === this.bestMove.orientation)
        {
            return;
        }
        this.bestMove = move;
        if (this.onChange) {
            this.onChange(move);
        }
    }
}

BotTask.REQUEST_INTERVAL = 100;
BotTask.MAX_REQUESTS = 20;

module.exports = { Bot, BotTask };

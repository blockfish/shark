const INITIAL = {
    status: 'initializing',
    error: null,
    move: null,
};

function reducer(bot, action) {
    switch (action.type) {
    case 'bot/error':
        return {...bot, status: 'error', error: action.payload};
    case 'bot/calculating':
        if (action.payload) {
            return {...bot, status: 'calculating'};
        } else {
            return {...bot, status: 'paused', move: null};
        }
    case 'bot/suggestion':
        {
            let moves = action.payload;
            if (moves.length === 0) {
                return bot.move === null ? bot : {...bot, move: null};
            }
            let newMove = moves[0];
            if (bot.move
                && bot.move.type === newMove.type
                && bot.move.x === newMove.x
                && bot.move.y === newMove.y
                && bot.move.orientation === newMove.orientation)
            {
                return bot;
            }
            return {...bot, move: newMove};
        }
    default:
        return bot;
    }
}

let error = dispatch => reason => dispatch({type:'bot/error', payload: reason});
let calculating = dispatch => state => dispatch({type:'bot/calculating', payload: state});
let suggestion = dispatch => moves => dispatch({type:'bot/suggestion', payload: moves});

module.exports = {
    INITIAL,
    reducer,
    error,
    calculating,
    suggestion,
};

const INITIAL = {
    auto: false,
    garbage: false,
};

function reducer(play, /* board, etc. */ action) {
    switch (action.type) {
    case 'play/toggleGarbage':
        // commitCurrentEdit(): FIXME(iitalics)
        // TODO(iitalics): if true then generate new garbage/queue immediately
        return {...play, garbage: !play.garbage};
    case 'play/toggleAuto':
        // commitCurrentEdit(): FIXME(iitalics)
        return {...play, auto: !play.auto};
    case 'play/step':
        // commitCurrentEdit(): FIXME(iitalics)
        // TODO(iitalics): accomplish... somehow?
    default:
        return play;
    }
}

let toggleGarbage = dispatch => () => dispatch({type: 'play/toggleGarbage'});
let toggleAuto = dispatch => () => dispatch({type: 'play/toggleAuto'});
let step = dispatch => () => dispatch({type: 'play/step'});

module.exports = {
    INITIAL,
    reducer,
    toggleGarbage,
    toggleAuto,
    step,
};

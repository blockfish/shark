const INITIAL = {
    selected: 'G',
    fillRow: true,
};

function reducer(palette, action) {
    switch (action.type) {
    case 'palette/selectType':
        return {...palette, selected: action.payload};
    case 'palette/toggleFillRow':
        return {...palette, fillRow: !palette.fillRow};
    default:
        return palette;
    }
}

let toggleFillRow = dispatch => () => dispatch({type: 'palette/toggleFillRow'});
let selectType = dispatch => type => dispatch({
    type: 'palette/selectType',
    payload: type,
});

module.exports = {
    INITIAL,
    reducer,
    toggleFillRow,
    selectType,
};

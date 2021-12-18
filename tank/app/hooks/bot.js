function useBot(board, edit, dispatch) {
    let [status, setStatus] = React.useState('thinking');

    let isEditing = edit !== null;
    React.useEffect(() => {
        if (isEditing) {
            setStatus('paused');
        } else {
            console.log('current playfield: ', Array.from(board.playfield.getFilledCells()));
            setStatus('thinking');
        }
    }, [board, isEditing]);

    return React.useMemo(() => ({ status }), [status]);
}

module.exports = { useBot };

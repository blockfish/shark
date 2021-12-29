function buildKeyMap(onEnterPiece, onCancel) {
    let keyMap = new Map();
    for (let [key, piece] of [
        ['KeyI', 'I'],
        ['KeyJ', 'J'],
        ['KeyL', 'L'],
        ['KeyO', 'O'],
        ['KeyS', 'S'],
        ['KeyT', 'T'],
        ['KeyZ', 'Z'],
    ]) {
        keyMap.set(key, () => onEnterPiece(piece));
    }
    keyMap.set('Escape', onCancel);
    return keyMap;
}

function useKeyEvents(container, onEnterPiece, onCancel, onOtherKey) {
    let keyMap = React.useMemo(() => buildKeyMap(onEnterPiece, onCancel), [onEnterPiece, onCancel]);
    React.useEffect(() => {
        function onKeyEvent(event) {
            keyMap.has(event.code)
                ? keyMap.get(event.code)()
                : onOtherKey(event.code)
        }
        container.addEventListener('keydown', onKeyEvent);
        return () => {
            container.removeEventListener('keydown', onKeyEvent);
        };
    }, [container, keyMap, onOtherKey]);
}

module.exports = { useKeyEvents };

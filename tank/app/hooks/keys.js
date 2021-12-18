function buildKeyMap() {
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
        keyMap.set(key, { type: 'edit:enter-piece', piece });
    }
    keyMap.set('Escape', { type: 'edit:cancel' });
    return keyMap;
}

function useKeyEvents(container, dispatch) {
    let keyMapRef = React.useRef();
    let keyMap = (keyMapRef.current ||= buildKeyMap());

    React.useEffect(() => {
        function onKeyEvent(event) {
            dispatch(
                keyMap.has(event.code)
                    ? keyMap.get(event.code)
                    : { type: 'other-key', code: event.code }
            );
        }

        container.addEventListener('keydown', onKeyEvent);
        return () => {
            container.removeEventListener('keydown', onKeyEvent);
        };
    }, [container, dispatch]);
}

module.exports = { useKeyEvents };

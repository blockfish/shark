const { CheckBox, IconButton } = require('../components/basic-widgets');

let BottomBar = ({ autoplay, bot, dispatch }) => {
    let toggleGarbage = React.useCallback(() => dispatch({type: 'play:toggle-garbage'}), [dispatch]);
    let togglePlay = React.useCallback(() => dispatch({type: 'play:toggle-play'}), [dispatch]);
    let stepForward = React.useCallback(() => dispatch({type: 'play:step-forward'}), [dispatch]);
    let undo = React.useCallback(() => dispatch({type: 'undo'}), [dispatch]);
    let redo = React.useCallback(() => dispatch({type: 'redo'}), [dispatch]);
    return (
        <div className="gi2-2 flex h hp2 hg2 sf">
            <div className="flex h hg1 vm2">
                <IconButton style="pr" size="lg" icon="undo" onClick={undo} />
                <IconButton style="pr" size="lg" icon="redo" onClick={redo} />
            </div>
            <div className="flex h hg1 vm2">
                <IconButton style="pr" size="lg" icon={autoplay.playing ? "pause" : "play"} onClick={togglePlay} />
                <IconButton style="pr" size="lg" icon="forward" onClick={stepForward} />
            </div>
            <div className="vmc">
                <CheckBox style="pr" size="md" checked={autoplay.garbage} onToggle={toggleGarbage}>
                    Add garbage automatically
                </CheckBox>
            </div>
            <div className="vmc">
                <p>Bot: <em>{`${bot.status}`}</em></p>
            </div>
        </div>
    );
};

module.exports = { BottomBar };

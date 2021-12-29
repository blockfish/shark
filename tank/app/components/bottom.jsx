const { CheckBox, IconButton } = require('../components/basic-widgets');

let BottomBar = ({ play, bot, onToggleGarbage, onToggleAuto, onStep }) => {
    let undo = () => console.log('undo not implemented');
    let redo = () => console.log('redo not implemented');
    return (
        <div className="gi2-2 flex h hp2 hg2 sf">
            <div className="flex h hg1 vm2">
                <IconButton style="pr" size="lg" icon="undo" onClick={undo} />
                <IconButton style="pr" size="lg" icon="redo" onClick={redo} />
            </div>
            <div className="flex h hg1 vm2">
                <IconButton style="pr" size="lg" icon={play.auto ? "pause" : "play"} onClick={onToggleAuto} />
                <IconButton style="pr" size="lg" icon="forward" onClick={onStep} />
            </div>
            <div className="vmc">
                <CheckBox style="pr" size="md" checked={play.garbage} onToggle={onToggleGarbage}>
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

const { Canvas } = require('../components/basic-widgets');
const { QueueView } = require('../views/queue');
const { PlayfieldView } = require('../views/playfield');
const theme = require('../theme');
const rules = require('../rules');

let Board = ({ board, edit, dispatch }) => {
    let viewsRef = React.useRef();
    let views = (viewsRef.current ||= {
        hold: new QueueView(1),
        playfield: new PlayfieldView(),
        queue: new QueueView(rules.previews),
    });

    // display intermediate states during an edit instead of the actual state
    let displayHold = board.hold;
    let displayQueue = board.queue;
    let displayPlayfield = board.playfield;
    let editingHold = null;
    let editingQueue = null;
    switch (edit && edit.type) {
    case 'draw':
        displayPlayfield = edit.playfield;
        isEditing = true;
        break;
    case 'enter-hold':
        displayHold = edit.hold;
        editingHold = 0;
        isEditing = true;
        break;
    case 'enter-queue':
        displayQueue = edit.queue;
        editingQueue = edit.queue.length;
        isEditing = true;
        break;
    }

    React.useEffect(() => {
        views.hold.setPiece(0, displayHold);
        views.hold.setEditing(editingHold);
        for (let i = 0; i < rules.previews; i++) {
            views.queue.setPiece(i, i < displayQueue.length ? displayQueue[i] : null);
        }
        views.queue.setEditing(editingQueue);
    }, [editingHold, editingQueue, displayHold, displayQueue]);

    views.hold.onClick = () => dispatch({ type: 'edit:click-hold' });
    views.queue.onClick = () => dispatch({ type: 'edit:click-queue' });

    React.useEffect(() => {
        views.playfield.setCells(displayPlayfield.getFilledCells());
        // TODO(iitalics): ghost cells from current bot suggestion
    }, [displayPlayfield]);

    views.playfield.onDragCursor = (dragType, x, y) => dispatch({ type: `edit:drag:${dragType}`, x, y });

    return (
        <div className="gi2-1 flex h align-start hm3 vm3 hg2">
            <Canvas view={views.hold} />
            <Canvas view={views.playfield} />
            <Canvas view={views.queue} />
        </div>
    );
};

module.exports = { Board };

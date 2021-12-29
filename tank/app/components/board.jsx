const { Canvas } = require('../components/basic-widgets');
const { QueueView } = require('../views/queue');
const { PlayfieldView } = require('../views/playfield');
const theme = require('../theme');
const rules = require('../rules');

let Board = ({ board, edit, bot, onClickHold, onClickQueue, onDrag }) => {
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
    case 'hold':
        displayHold = edit.hold;
        editingHold = 0;
        isEditing = true;
        break;
    case 'queue':
        displayQueue = edit.queue;
        editingQueue = edit.queue.length;
        isEditing = true;
        break;
    }

    let displayGhostCells = React.useMemo(() => {
        if (bot.move) {
            return getMoveCells(bot.move.type, bot.move.x, bot.move.y, bot.move.orientation);
        } else {
            return [];
        }
    }, [bot.move]);

    React.useEffect(() => {
        views.hold.setPiece(0, displayHold);
        views.hold.setEditing(editingHold);
        for (let i = 0; i < rules.previews; i++) {
            views.queue.setPiece(i, i < displayQueue.length ? displayQueue[i] : null);
        }
        views.queue.setEditing(editingQueue);
    }, [editingHold, editingQueue, displayHold, displayQueue]);

    React.useEffect(() => {
        views.playfield.setCells(displayPlayfield.getFilledCells());
        views.playfield.setGhost(displayGhostCells);
    }, [displayPlayfield, displayGhostCells]);

    views.hold.onClick = onClickHold;
    views.queue.onClick = onClickQueue;
    views.playfield.onDragCursor = onDrag;

    return (
        <div className="gi2-1 flex h align-start hm3 vm3 hg2">
            <Canvas view={views.hold} />
            <Canvas view={views.playfield} />
            <Canvas view={views.queue} />
        </div>
    );
};

function getMoveCells(type, x0, y0, orientation) {
    let xx = 0, yx = 0, xy = 0, yy = 0;
    switch (orientation) {
    case 'north': xx = 1; yy = 1; break;
    case 'east': xy = -1; yx = 1; break;
    case 'south': xx = -1; yy = -1; break;
    case 'west': xy = 1; yx = -1; break;
    }
    return rules.shapes[type].map(([dx, dy]) => {
        let x = x0 + xx * dx + yx * dy;
        let y = y0 + xy * dx + yy * dy;
        return { x, y, type };
    });
}

module.exports = { Board };

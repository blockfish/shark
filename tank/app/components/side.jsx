const { CheckBox, Canvas } = require('../components/basic-widgets');
const { ColorPadView } = require('../views/color');

function buildViews() {
    let views = {};
    for (let type of ColorPadView.ALL_TYPES) {
        views[type] = new ColorPadView(type);
    }
    return views;
}

let ColorPalette = ({ layout, selected, onSelect }) => {
    let viewsRef = React.useRef();
    let views = (viewsRef.current ||= buildViews());

    React.useEffect(() => {
        for (let type of Object.keys(views)) {
            views[type].setSelected(type === selected);
        }
    }, [selected]);

    let items = [];
    for (let y = 0; y < layout.length; y++) {
        for (let x = 0; x < layout[y].length; x++) {
            let type = layout[y][x];
            let view = views[type];
            view.onClick = () => onSelect(type);
            items.push(
                <div
                    style={{gridColumn: x+1, gridRow: y+1}}
                    key={type}
                >
                    <Canvas view={view} />
                </div>
            );
        }
    }

    return (
        <div className="grid vg1 hg1">
            {items}
        </div>
    );
};

let SideBar = ({ palette, dispatch }) => {
    let selectType = React.useCallback(select => dispatch({type: 'palette:select-type', select}), [dispatch]);
    let toggleFillRow = React.useCallback(() => dispatch({type: 'palette:toggle-fill-row'}), [dispatch]);
    return (
        <div className="gi1-1 flex v vp2 vg2 sf">
            <div className="hm2">
                <ColorPalette
                    layout={[
                        ["Z", "J"],
                        ["L", "I"],
                        ["O", "T"],
                        ["S", "G"],
                        ["auto"],
                    ]}
                    selected={palette.selected}
                    onSelect={selectType} />
            </div>
            <div className="flex v vg1 hm1">
                <CheckBox style="pr" size="md" checked={palette.fillRow} onToggle={toggleFillRow}>
                    Fill row
                </CheckBox>
            </div>
        </div>
    );
};

module.exports = { SideBar };

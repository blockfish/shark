const ICON_CHARS = {
    check:    "\uF00C",
    redo:     "\uF01E",
    backward: "\uF04A",
    play:     "\uF04B",
    pause:    "\uF04C",
    forward:  "\uF04E",
    undo:     "\uF0E2",
};

let Button = ({ style, size, onClick, children }) => (
    <button className={`${style||"pr"} ${size||"md"}`} onClick={onClick}>
        <span>{children}</span>
    </button>
);

let IconButton = ({ style, size, icon, onClick }) => (
    <button className={`icon ${style||"pr"} ${size||"md"}`} onClick={onClick}>
        <span>{ICON_CHARS[icon]}</span>
    </button>
);

let CheckBox = ({ style, size, checked, onToggle, children }) => (
    <label className={`checkbox ${checked?"ch":"uc"} ${style||"pr"} ${size||"md"}`}>
        <button className="icon" onClick={onToggle}><span>{ICON_CHARS.check}</span></button>
        <span>{children}</span>
    </label>
);

let Canvas = ({ view }) => {
    let onCanvas = React.useCallback(canvas => view.bind(canvas), [view]);
    return (<canvas ref={onCanvas} />);
};

module.exports = { Button, IconButton, CheckBox, Canvas };

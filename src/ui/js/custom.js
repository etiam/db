function property(param, value) {
    if (arguments.length == 1) {
        // Get
        return sessionStorage[param];
    } else {
        // Set
        sessionStorage[param] = value;
    }
}

property("lines", 1);

// All necessary calls here
editor.on('change', function() {
    var newLines = editor.session.getLength();
    if (newLines != property("lines")) {
        property("lines", newLines);
    }
});

editor.on('mousemove', function(e) {
    sc = editor.renderer.pixelToScreenCoordinates(e.clientX, e.clientY);                                                                                          
    codeview.onMouseMove(sc.row + 1, sc.column + 1);
});

// gutter width
var mywidth = 3;
editor.session.gutterRenderer = {
    setWidth: function(width) {
        mywidth = width;
    },
    getWidth: function(session, lastLineNumber, config) {
        return mywidth * config.characterWidth;
    },
    getText: function(session, row) {
        return row+1;
    }
};

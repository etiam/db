// not sure yet
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
property("text", "");

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


// global value store
function property(param, value) 
{
    if (arguments.length == 1) 
    {
        // Get
        return sessionStorage[param];
    } else 
    {
        // Set
        sessionStorage[param] = value;
    }
}

// the number of lines in the document
property("lines", 1);
property("charheight", editor.renderer.characterHeight);

// calc number of lines when document changes
editor.on('change', function() 
{
    var newLines = editor.session.getLength();
    if (newLines != property("lines")) 
    {
        property("lines", newLines);
    }
});

// callback for mousemove
editor.on('mousemove', function(e) 
{
    sc = editor.renderer.pixelToScreenCoordinates(e.clientX, e.clientY);
    codeview.onMouseMoved(sc.row + 1, sc.column + 1);
});

// callback for keypress
editor.on("keyboardActivity", function(e) 
{
	cp = editor.getCursorPosition()
    codeview.onCursorMoved(cp.row + 1, cp.column + 1);
});

editor.on("guttermousedown", function(e) 
{
    var target = e.domEvent.target; 
    if (target.className.indexOf("ace_gutter-cell") == -1)
        return; 
    if (!editor.isFocused()) 
        return; 
    if (e.clientX > 25 + target.getBoundingClientRect().left) 
        return; 

    var row = e.getDocumentPosition().row;
    codeview.onGutterClicked(row + 1);
    e.stop();
});

// set constant gutter width
var mywidth = 3;
editor.session.gutterRenderer = 
{
    setWidth: function(width) 
    {
        mywidth = width;
    },
    getWidth: function(session, lastLineNumber, config) 
    {
        return mywidth * config.characterWidth;
    },
    getText: function(session, row) 
    {
        return row+1;
    }
};


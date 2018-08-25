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
property("currentlinemarker", ace.require('ace/range').Range);

var Range = ace.require('ace/range').Range;
var currmark;

function highlightline(row)
{
    var r = new Range(row, 0,row, 1); 
    currmark = editor.session.addMarker(r, "current-line-marker", "fullLine");
}

function unhighlightlast()
{
    if (currmark != null) 
    {
        editor.session.removeMarker(currmark);
    }
}

// calc number of lines in document when document changes
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
    index = editor.session.doc.positionToIndex(sc)
    codeview.onMouseMoved(index);
});

// callback for keypress
editor.on("keyboardActivity", function(e) 
{
	cp = editor.getCursorPosition()
    codeview.onCursorMoved(cp.row + 1, cp.column + 1);
});

// call into Editor c++ class on mouse click in gutter
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

/*
editor.container.addEventListener("contextmenu", function(e) 
{
    e.preventDefault();
    return false;
}, false);

function gutterContextMenu(e)
{
    e.preventDefault();
    alert('cuss!');
    return false;
}

var gutter = editor.renderer.$gutterLayer;

gutter.addListener("contextmenu", gutterContextMenu, false);
*/

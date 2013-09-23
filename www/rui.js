var uiList;
var screenIndex = 0;    // screen relative current (highlighted) index. (0:maxPanels-1)
var scrollIndex = 0;    // base index of screenIndex:0
var selectIndex = 0;    // effective index of corresponding rui element (screenIndex+scrollIndex)
var maxPanels = 4;      // max panels to be displayed (arbitrary, based on vendors screen size)
var panelCount = 0;     // actual panels to be displayed
var flipped = false;    // flipped == true means the 'back' elements are in front and the 'front' elements are in back
var uiElements;         // array of innerHtml content for each rui
var canScroll;          // uiElements.length > panelCount
var elementHeight = 86; // based on height of background images for the elements
var uiElementCount = 0;


function pageLoaded() {
    uiElements = [];

    // Page HTML with empty elements
    generatePage();

    // Establish callback for ui list updates
    navigator.getNetworkServices(["upnp:urn:schemas-upnp-org:service:RemoteUIServer:1"], refreshRUIList, handleError);
}

function updateSelected() {

    var elems = document.getElementsByClassName('uiElementNumber');
    var index;
    var i;

    for( i = 0; i < elems.length; i++)  {

        // Two faces per panel. Update them both.
        index = i >> 1;
        if (index === screenIndex) {
            $(elems[i]).addClass('selected');
        } else {
            $(elems[i]).removeClass('selected');
        }
    }

    elems = document.getElementsByClassName('uiElementName');
    for(i = 0; i < elems.length; i++)  {

        // Two faces per panel. Update them both.
        index = i >> 1;
        if (index === screenIndex) {
            $(elems[i]).addClass('selected');
        } else {
            $(elems[i]).removeClass('selected');
        }
    }
}

function generateRUIElements(services) {
    uiElements = [];

    for (var i = 0; i < services.length; i++) {
        var selected = (i == selectIndex) ? " selected" : "";

        var icon = selectIcon(uiList[index]);
        var iconURL = icon.url;

        // TODO: read style sheet instead of using constant
        var paddingTop = (elementHeight - icon.height) / 2;
        var paddingLeft = (elementHeight - icon.width) / 2;

        var displayNumber = (i + 1) % 10;

        var elementInnerHtml = "<div class='uiElementNumber" + selected + "'>";
        elementInnerHtml += displayNumber;
        elementInnerHtml += '</div>';

        elementInnerHtml += "<div class='uiElementName" + selected + "'>";
        elementInnerHtml += "<div class='uiElementIcon' ";
        elementInnerHtml += "style='padding-left:" + paddingLeft + "; padding-top: " + paddingTop + ";'>";
        elementInnerHtml += "<img src='" + iconURL + "'/>";
        elementInnerHtml += "</div>";

        elementInnerHtml += "<div class='uiElementText'>";
        elementInnerHtml += ui.name;
        elementInnerHtml += "</div>";
        elementInnerHtml += "</div>";

        uiElements[i] = {
            uiID: ui.uiID,
            html: elementInnerHtml
        };
    }

    canScroll = services.length > panelCount;
}

function generatePage() {
    var innerHTML = "";

    // Scroll up indicator
    innerHTML += "<div id='navHeader' class='arrowHidden' >";
    innerHTML += "</div>";

    // Determine how many panels we will display - and whether we can scroll or not.
    panelCount = maxPanels;

    for (var i=0; i < panelCount; i++) {

        //var yPos = 20 + (i*elementHeight);
        var yPos = 60 + (i*elementHeight);

        // Panel div
        innerHTML += "<div class='panel' onclick='selectPanel("+i+")'>";

        // Front div. RUI element will be inserted later.
        innerHTML += "<div class='uiElement front' style='top: "+yPos+"px;'></div>";

        // Back div. RUI element will be inserted later.
        innerHTML += "<div class='uiElement back' style='top: "+yPos+"px;'></div>";

        innerHTML += "</div>";
    }

    // Scroll down indicator
    innerHTML += "<div id='navFooter' class='arrowHidden' >";
    innerHTML += "</div>";

    innerHTML += "<div id='selectInstructions' >";
    innerHTML += "Select a Service"
    innerHTML += "</div>";



    document.getElementById('navUI').innerHTML = innerHTML;
}

function handleError(error) {
    console.log(error);
    window.alert(error);
}

// Load either the front or back faces.
// This is required for an initial load and prior to a flip animation.
function loadPanelElements(face, start) {
    if (uiElements.length == 0)
        return;

    var elems = document.getElementsByClassName(face);
    for(var i = 0; i < elems.length; i++)  {
        var currentHtml = elems[i].innerHTML;

        var index = start+i;

        var html = "";
        var uiID = "";
        var count = uiElements.length;

        if (index < count) {
            html = uiElements[index].html;
            uiID = uiElements[index].uiID;
        }

        if (face == "front") {
            $(elems[i]).html(html);
        }
        else if (face == "back") {
            $(elems[i]).html(html);
        }
    }
}

// Here with an update list of RUIs
function refreshRUIList(services) {

    // Array of innerHtml elements
    generateRUIElements(services);

    // Insert element HTML
    loadPanelElements('front', scrollIndex);
    loadPanelElements('back', scrollIndex);

    // Set selection
    updateSelected();
}

// TODO: Select from the list based on best size match.
// For now just grab the first one.
function selectIcon(ui) {

    if (ui.iconList.length > 0) {
        return ui.iconList[0];
    }

    return {
        width: 0,
        height: 0,
        url: "rui_missingIcon.png"
    };
}

function scrollUp() {

    // Populate back facing panes before we animate flip
    scrollIndex--;
    var face = flipped ? 'front' : 'back';
    loadPanelElements(face, scrollIndex);

    flipped = !flipped;
    var elems = document.getElementsByClassName('panel');
    for(i=0; i<elems.length; i++)  {
        if ( flipped ) {
            $(elems[i]).addClass('flip');
        } else {
            $(elems[i]).removeClass('flip');
        }
    }
}

function scrollDown() {

    // Populate back facing panes before we animate flip
    scrollIndex++;
    var face = flipped ? 'front' : 'back';
    loadPanelElements(face, scrollIndex);

    flipped = !flipped;

    var elems = document.getElementsByClassName('panel');
    for(var i = 0; i < elems.length; i++)  {
        if (flipped) {
            $(elems[i]).addClass('flip');
        } else {
            $(elems[i]).removeClass('flip');
        }
    }
}

function recordScreenPosition() {
    selectIndex = screenIndex + scrollIndex;
    updateSelected();
}

function onKeydown(ev) {

    key = (ev.which || ev.keyCode);

    switch(key) {

    case 13:
        // enter
        selectUI(selectIndex);
        break;

    case 37:
        // left arrow
        break;

    case 38:
        // up arrow
        if (screenIndex > 0) {
            screenIndex--;
            recordScreenPosition();
        } else if (canScroll && (scrollIndex > 0)) {
            scrollUp();
            recordScreenPosition();
        }
        ev.preventDefault();
        break;

    case 39:
        // right arrow
        break;

    case 40:
        // down arrow
        if (screenIndex < (panelCount-1) && screenIndex < (uiElementCount-1)) {
            screenIndex++;
            recordScreenPosition();
        } else if (canScroll && (scrollIndex < (uiElements.length-panelCount))) {
            scrollDown();
            recordScreenPosition();
        }
        ev.preventDefault();
        break;

    default:
        if (key >= 48 && key <= 57) {
            var numKey = key - 48;
            var index = numKeyToIndex(numKey);
            if (index >= 0 && index < uiList.length) {
                screenIndex = index - scrollIndex;
                recordScreenPosition();
                selectUI(index);
            }
        }
        break;
    }
}

function numKeyToIndex(numKey) {

    for (var i = 0; i < panelCount; i++) {
        var index = scrollIndex + i;
        var displayIndex = (index + 1) % 10;
        if (numKey === displayIndex) {
            return index;
        }
    }

    return -1;
}

// Here when a RUI panel was clicked.
function selectPanel(index) {
    screenIndex = index;
    recordScreenPosition();
    selectUI(selectIndex)
}

function selectUI(index) {
    if (index < uiList.length) {
        var uri = uiList[index].protocolList[0].uriList[0];
        window.location.href = uri;
    }
}

window.onload = pageLoaded;

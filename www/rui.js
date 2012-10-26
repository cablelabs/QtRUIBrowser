var uiList;

function pageLoaded() {

    proxyConnect();

    // Fetch initial list
    refreshRUIList();
}

function proxyConnect() {

    // Establish callback for ui list updates
    discoveryProxy.ruiListNotification.connect(refreshRUIList);
    //alert("proxyConnect");
}

function refreshRUIList() {

    uiList = discoveryProxy.ruiList();
    var count = uiList.length;
    var innerHTML = "";

    // Scroll up arrow
    innerHTML += "<div id='navHeader' class='arrowHidden' >";
    //innerHTML += "<img src='qrc:/www/rui_arrowUp.png' />"
    innerHTML += "</div>";

    // There can be multiple urls for a given ui, which will result in additional divs
    var divCount = 0;

    for (var i=0; i < count; i++) {

        var index = i;
        //discoveryProxy.console("JS: UI(" + index + ")\n");

        var generateUIDiv = function(ui) {

            var icon = selectIcon(ui);
            var iconURL = icon.url;
            var elementHeight = 86;

            // TODO: read style sheet instead of using constant
            var paddingTop = (elementHeight - icon.height) / 2;
            var paddingLeft = (elementHeight - icon.width) / 2;

            var html = "";
            var displayNumber = divCount + 1;

            // Outer div
            html += "<div class='uiElement' onclick='selectUI(" + displayNumber + ")'>";
            {
                html += "<div class='uiElementNumber'>";
                html += displayNumber;
                html += '</div>';
                divCount++;


                html += "<div class='uiElementName'>";
                {
                    html += "<div class='uiElementIcon' ";
                    html += "style='padding-left:" + paddingLeft + "; padding-top: " + paddingTop + ";'>";
                    html += "<img src='" + iconURL + "'/>";
                    html += "</div>";

                    html += "<div class='uiElementText'>";
                    html += ui.name;
                    html += "</div>";
                }
                html += "</div>";

            }
            html += "</div>";
            return html;
        }

        innerHTML += generateUIDiv(uiList[index]);
    }

    // Scroll down arrow
    innerHTML += "<div id='navFooter' class='arrowHidden' >";
    //innerHTML += "<img src='qrc:/www/rui_arrowDown.png' />"
    innerHTML += "</div>";

    //discoveryProxy.console("InnerHTML: " + innerHTML);

    document.getElementById('navUI').innerHTML = innerHTML;
}

function selectIcon(ui) {

    if (ui.iconList.length > 0) {
        return ui.iconList[0];
    }

    var icon = new Object;
    icon.width = 0;
    icon.height = 0;
    icon.url = "";

    return icon;
}

window.onload = pageLoaded;

function onKeydown(ev) {

    key=((ev.which)||(ev.keyCode));

    switch(key) {

    case 37:
        // left arrow
        break;

    case 38:
        // up arrow
        break;

    case 39:
        // right arrow
        break;

    case 40:
        // down arrow
        break;

    case 27:
        // escape (home)
        break;

    default:

        if ( key >= 48 && key <= 57 ) {
            var numKey = key - 48;
            selectUI(numKey);
        } else {

            //alert(key);
        }

        break;

    }
}

function selectUI(numKey) {

    // numKey is one based.
    var index = numKey;
    if (index == 0)
        index = 10;
    index--;

    if (index < uiList.length) {

        var uri = uiList[index].protocolList[0].uriList[0];

        discoveryProxy.console("select ui: " + index + "  " + uiList[index].name + "  url: " + uri );
        window.location.href = uri;
    }
}


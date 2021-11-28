// Supports context help

updateHelp(null); // Set context-sensitive help

function updateHelp(hashLoc = null) {
    var thisPath = window.location.pathname // Path of current page
    var url = "https://docs.beesnuggler.com";
    var url = url + "/en/latest/context";
    url = url + window.location.pathname;

    // Get context help for settings page
    if (hashLoc && window.location.pathname == "/settings/") {
        // Switch here for hashLoc
        switch (hashLoc) {
            case "#controller":
                url = url + "controller/";
                break;
            case "#tempcontrol":
                url = url + "control/";
                break;
            case "#sensorcontrol":
                url = url + "sensors/";
                break;
            case "#reset":
                url = url + "reset/";
                break;
            case "#update":
                url = url + "update/";
                break;
            case "#wifi":
                url = url + "wifi/";
                break;
            default:
                // Unknown hash location passed
                break;
        }
    } else {
        // Not on a page with a hashloc
    }

    url = url + "index.html";

    if (!navigator.onLine) {
        $("#contexthelp").prop("href", "/offline/");
    } else if (window.location.pathname.endsWith(".htm")) {
        $("#contexthelp").prop("href", "/notavail/");
    } else {
        $("#contexthelp").prop("href", url);
    }
}

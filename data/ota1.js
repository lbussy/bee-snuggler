// Supports OTA1 page

var unloadingState = false;
var numReq = 2;
var loaded = 0;
document.getElementById("proceed").disabled = true;

// Attach the event after the page loads (multi-browser)
if (window.addEventListener) window.addEventListener("load", loadPage, false);
else if (window.attachEvent) window.attachEvent("onload", loadPage);
else window.onload = loadPage;

// Detect unloading state during getJSON
$(window).bind("beforeunload", function () {
    unloadingState = true;
});

function loadPage() {
    // Page data load - make triple sure the whole DOM loaded
    if (document.readyState === "ready" || document.readyState === "complete") {
        populatePage();
    } else {
        document.onreadystatechange = function () {
            if (document.readyState == "complete") {
                populatePage();
            }
        };
    }
}

function populatePage() { // Get page data
    loadThisVersion(); // Populate form with controller settings
    loadThatVersion(); // Populate form with controller settings
    pollComplete();
}

function loadThisVersion() { // Get current parameters
    var thisVersionJson = "/api/v1/info/thisVersion/";
    var thisVersion = $.getJSON(thisVersionJson, function () {
    })
        .done(function (thisVersion) {
            try {
                $('#thisFWVersion').text(thisVersion.fw_version);
                $('#thisFSVersion').text(thisVersion.fs_version);
            }
            catch {
                $('#thisFWVersion').text("Error loading.");
                $('#thisFSVersion').text("Error loading.");
            }
        })
        .fail(function () {
            $('#thisFWVersion').text("Error loading.");
            $('#thisFSVersion').text("Error loading.");
        })
        .always(function () {
            // Can post-process here
            loaded++;
        });
}

function loadThatVersion() { // Get current parameters
    var thatVersionJson = "/api/v1/info/thatVersion/";
    var thatVersion = $.getJSON(thatVersionJson, function () {
    })
        .done(function (thatVersion) {
            try {
                $('#thatFWVersion').text(thatVersion.fw_version);
                $('#thatFSVersion').text(thatVersion.fs_version);
            }
            catch {
                $('#thatFWVersion').text("Error loading.");
                $('#thatFSVersion').text("Error loading.");
            }
        })
        .fail(function () {
            $('#thatFWVersion').text("Error loading.");
            $('#thatFSVersion').text("Error loading.");
        })
        .always(function () {
            // Can post-process here
            loaded++;
        });
}

function pollComplete() {
    if (loaded == numReq) {
        finishPage();
    } else {
        setTimeout(pollComplete, 300); // try again in 300 milliseconds
    }
}

function finishPage() { // Display page
    document.getElementById("proceed").disabled = false;
}

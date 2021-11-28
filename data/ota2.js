// Supports OTA

// Attach the event after the page loads
if (window.addEventListener)
    window.addEventListener("load", loadPage, false);
else if (window.attachEvent)
    window.attachEvent("onload", loadPage);
else window.onload = loadPage;

function loadPage() { // Load the page's JS elements
    // TODO"  Let these detect failures
    $.ajax({
        url: '/clearupdate/',
        type: 'PUT',
        success: function(result) {}
    });
    $.ajax({
        url: '/updatestart/',
        type: 'PUT',
        success: function(result) {}
    });

    // Wait for update to complete
    var running = false;
    var intervalID = window.setInterval(function () { // Poll every 5 seconds
        checkSemaphore(function (semaphore) {
            didupdate = semaphore;
            if ((didupdate == true) && (running == false)) {
                running = true;
                // Update is complete
                $.ajax({
                    url: '/clearupdate/',
                    type: 'PUT',
                    success: function(result) {}
                });
                $("#banner").replaceWith("<h1>Firmware Update Complete</h1>");
                $("#subtitle").replaceWith("<h4 class='card-header' class='card-title'>Redirect Pending</h4>");
                $("#message").replaceWith("<p class='card-body'>The firmware update is complete.  You will be redirected momentarily.</p>");
                setTimeout(function () { window.location.href = "/"; }, 5000);
            }
        });
    }, 5000);
}

function checkSemaphore(callback) { // Check to see if the update is complete
    var jqxhr = $.getJSON("/api/v1/action/ping/")
        .done(function (data) {
            callback(data.didupdate);
        })
        .fail(function () {
            // This will fail while controller resets
            callback({});
        }
        );
}
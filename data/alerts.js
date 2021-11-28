// Common file for AJAX errors

// Create a constant to hold an alert banner template
const alert = '<div class="alert alert-dismissible alert-%TYPE%" id="%ID%" fade show>' + 
    '<button type="button" class="btn-close" data-dismiss="alert"></button>' + 
    '<p class="mb-0"><strong>%HEADER%: </strong>%MESSAGE%</p></div>'

settingsAlert = function () { }
settingsAlert.warning = function (message) {
    if (message) {
        var type = "warning";
        var id = "warnSettingsWarn";
        var header = "Settings Warning";
        var localAlert = alert.replace("%TYPE%", type);
        localAlert = localAlert.replace("%ID%", id);
        localAlert = localAlert.replace("%HEADER%", header);
        localAlert = localAlert.replace("%MESSAGE%", message);
        $('#settingsAlert_placeholder').html(localAlert);
    } else {
        $('#settingsAlert_placeholder').html("");
    }
};
settingsAlert.error = function (message) {
    if (message) {
        var type = "error";
        var id = "warnSettingsError";
        var header = "Settings Error";
        var localAlert = alert.replace("%TYPE%", type);
        localAlert = localAlert.replace("%ID%", id);
        localAlert = localAlert.replace("%HEADER%", header);
        localAlert = localAlert.replace("%MESSAGE%", message);
        $('#settingsAlert_placeholder').html(localAlert);
    } else {
        $('#settingsAlert_placeholder').html("");
    }
};

tempsAlert = function () { }
tempsAlert.warning = function (message) {
    if (message) {
        var type = "warning";
        var id = "warnTempError";
        var header = "Temps Warning";
        var localAlert = alert.replace("%TYPE%", type);
        localAlert = localAlert.replace("%ID%", id);
        localAlert = localAlert.replace("%HEADER%", header);
        localAlert = localAlert.replace("%MESSAGE%", message);
        $('#tempAlert_placeholder').html(localAlert);
    } else {
        $('#tempAlert_placeholder').html("");
    }
};

configAlert = function () { }
configAlert.warning = function (message) {
    if (message) {
        var type = "warning";
        var id = "warnConfigError";
        var header = "Config Warning";
        var localAlert = alert.replace("%TYPE%", type);
        localAlert = localAlert.replace("%ID%", id);
        localAlert = localAlert.replace("%HEADER%", header);
        localAlert = localAlert.replace("%MESSAGE%", message);
        $('#configAlert_placeholder').html(localAlert);
    } else {
        $('#configAlert_placeholder').html("");
    }
};

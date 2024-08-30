// NHC discussion next/prev

// Increment
javascript: (function() {
    var currentUrl = window.location.href;
    var match = currentUrl.match(/(nhc\.noaa\.gov.*)\.(\d+)(\.shtml|\.html)?(\?)?/);
    if (match) {
        var newNumber = parseInt(match[2], 10) + 1;
        var newUrl = currentUrl.replace(/(nhc\.noaa\.gov.*)\.(\d+)(\.shtml|\.html)?(\?)?/, `$1.${newNumber.toString().padStart(3, '0')}$3$4`);
        window.location.href = newUrl;
    } else {
        alert('URL format not recognized.');
    }
})();

// Decrement
javascript: (function() {
    var currentUrl = window.location.href;
    var match = currentUrl.match(/(nhc\.noaa\.gov.*)\.(\d+)(\.shtml|\.html)?(\?)?/);
    if (match) {
        var newNumber = parseInt(match[2], 10) - 1;
        var newUrl = currentUrl.replace(/(nhc\.noaa\.gov.*)\.(\d+)(\.shtml|\.html)?(\?)?/, `$1.${newNumber.toString().padStart(3, '0')}$3$4`);
        window.location.href = newUrl;
    } else {
        alert('URL format not recognized.');
    }
})();

// Sentence case
javascript: (function() {
    function toSentenceCase(str) {
        return str.toLowerCase().replace(/(^\s*\w|[.!?]\s+(?<!\.\.\.)\w|(\n\s*\n)\s*\w)/g, function(c) {
            return c.toUpperCase();
        });
    }

    function convertPreElements() {
        // Select all <pre> elements
        var preElements = document.querySelectorAll('pre');
        
        preElements.forEach(function(pre) {
            // Iterate over each text node within the <pre> element
            pre.childNodes.forEach(function(node) {
                if (node.nodeType === Node.TEXT_NODE) {
                    // Convert the text node content to sentence case
                    node.nodeValue = toSentenceCase(node.nodeValue);
                }
            });
        });
    }

    convertPreElements();
})();

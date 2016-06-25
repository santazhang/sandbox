function removeAllIframes() {
    Array.prototype.slice.call(document.querySelectorAll('iframe')).forEach(function(element, index, array) {
        element.parentNode.removeChild(element);
    });
}

setInterval(removeAllIframes, 1000);

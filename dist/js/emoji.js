// Get emojis from file
function get_emojo_picker(callback)
{
    let xhr = new XMLHttpRequest();
    xhr.open("GET", "/emoji_picker");
    xhr.onreadystatechange = function() {
        if (this.readyState === XMLHttpRequest.DONE)
            callback(this.response);
    };
    xhr.send();
}

get_emojo_picker((emojo_picker_str) => {
    document.body.innerHTML += emojo_picker_str;
});

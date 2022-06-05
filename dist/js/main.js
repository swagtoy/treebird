Element.prototype.insertAfter = function(element) {
    element.parentNode.insertBefore(this, element.nextSibling);
};



function construct_quick_reply_form(replyid)
{
    let src = document.createElement("form");
    src.action = "/status/create";
    src.method = "post";
    src.enctype = "multipart/form-data";
    src.className = "statusbox-quickreply";
    let hiddeninput = document.createElement("input");
    hiddeninput.type = "hidden";
    hiddeninput.name = "replyid"
    hiddeninput.value = replyid;
    let statusbox = document.createElement("div");
    statusbox.className = "statusbox statusbox-ani";

    let textarea = document.createElement("textarea");
    textarea.placeholder = "Just landed in N.Y.";
    textarea.rows = 5;
    textarea.tabindex = 1;
    textarea.name = "content";

    let statusfooter = document.createElement("div");
    statusfooter.className = "statusfooter";
    let statusfooter_sides = {
        left: document.createElement("div"),
        right: document.createElement("div"),
    }
    let select = document.createElement("select");
    let files_input = document.createElement("input");
    statusfooter_sides.left.className = "statusfooter-left";
    select.innerHTML = `
          <option value="public">Public</option>
          <option value="unlisted">Unlisted</option>
          <option value="private">Private</option>
          <option value="direct">Direct</option>
          <option value="local">Local</option>
    `.trim();
    files_input.type = "file";
    files_input.name = "file";
    files_input.tabindex = 4;
    files_input.multiple = "";

    statusfooter_sides.right.className = "statusfooter-right";
    let submitbtn = document.createElement("input");
    submitbtn.className = "btn post-btn";
    submitbtn.type = "submit";
    submitbtn.value = "Post";
    submitbtn.tabindex = 2;

    
    statusfooter_sides.left.appendChild(select);
    statusfooter_sides.left.appendChild(files_input);
    statusfooter_sides.right.appendChild(submitbtn);
    statusfooter.appendChild(statusfooter_sides.left);
    statusfooter.appendChild(statusfooter_sides.right);
    statusbox.appendChild(textarea);
    statusbox.appendChild(statusfooter);
    src.appendChild(hiddeninput);
    src.appendChild(statusbox);
    return src;
}

function create_reply_form(e)
{
    e.preventDefault();
    let status = e.target.closest(".status");

    if (status.nextSibling.className === "statusbox-quickreply")
    {
        status.nextSibling.remove();
    }
    else {
        let form = construct_quick_reply_form(status.id);
        form.insertAfter(status);
        
    }
    
    
    return false;
}

// Main
(function() {
    let reply_btn = document.getElementsByClassName("reply-btn");

    for (let i = 0; i < reply_btn.length; ++i)
    {
        reply_btn[i].addEventListener('click', create_reply_form);
    }
})();

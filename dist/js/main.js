(function(){
    // Global state variable
    let state = {
        file: {
            files: {},
            file_counter: 0,
        }
    };
    
    Element.prototype.insertAfter = function(element) {
        element.parentNode.insertBefore(this, element.nextSibling);
    };

    function get_cookie(cookiestr)
    {
        return document.cookie
            .split(';')
            .find(row => row.startsWith(cookiestr+'='))
            .split('=')[1];
    }

    // TODO Check if logged in .acct value is the same
    function reply_get_mentions(reply, content)
    {
        const regexpr = /<a target="_parent" class="mention" href="https?:\/\/.*?\/@(.*?)@(.*?)">.*?<\/a>/g;
        const arr = [...content.matchAll(regexpr)];
        let res = reply ? `@${reply} ` : "";
        const matches = content.matchAll(regexpr);

        for (let x of matches)
        {
            res += `@${x[1]}@${x[2]} `;
        }
        return res;
    }

    function form_enter_submit(e, that)
    {
        if ((e.ctrlKey || e.metaKey) && e.keyCode === 13)
            that.closest('form').submit();
    }

    // Submit form entry on enter when in textbox/textarea
    document.querySelectorAll("input[type=text], input[type=url], input[type=email], input[type=password], textarea").forEach((i) => {
        i.addEventListener("keydown", e => form_enter_submit(e, i));
    });


    function construct_quick_reply_form(status)
    {
        let src = document.createElement("form");
        src.action = "/status/create";
        src.method = "post";
        src.enctype = "multipart/form-data";
        src.className = "statusbox-quickreply";
        let hiddeninput = document.createElement("input");
        hiddeninput.type = "hidden";
        hiddeninput.name = "replyid"
        hiddeninput.value = status.id;
        let statusbox = document.createElement("div");
        statusbox.className = "statusbox statusbox-ani";

        let textarea = document.createElement("textarea");
        textarea.placeholder = "Just landed in N.Y.";
        textarea.rows = 5;
        textarea.tabindex = 1;
        textarea.name = "content";

        // Load placeholder text
        let instance_info = status.querySelector(".instance-info");
        textarea.innerText = reply_get_mentions(
            instance_info ? instance_info.innerText : null,
            status.querySelector(".status-content").innerHTML
        );

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
        textarea.addEventListener("keydown", e => form_enter_submit(e, textarea));
        statusbox.appendChild(statusfooter);
        src.appendChild(hiddeninput);
        src.appendChild(statusbox);
        return src;
    }

    function construct_query(query)
    {
        query_string = "";
        let keys = Object.keys(query);
        let vals = Object.values(query);
        const len = keys.length;

        for (let i = 0; i < keys.length; ++i)
        {
            query_string += keys[i] + "=" + vals[i];
            if (i !== keys.length-1)
                query_string += "&";
        }
        return query_string;
    }

    function send_request(url, query, type, cb, cb_args)
    {
        let query_str = construct_query(query);
        let xhr = new XMLHttpRequest();
        xhr.open("POST", url);
        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        xhr.onreadystatechange = function() {
            if (this.readyState === XMLHttpRequest.DONE)
                cb(this, cb_args);
        };
        xhr.send(query_str);
    }

    function change_count_text(val, sum)
    {
        if (val === "")
            val = 0
        else
            val = parseInt(val);
        val += sum;
        return val > 0 ? val.toString() : "";
    }

    function interact_action(status, type)
    {
        let like = status.querySelector(".statbtn .like");
        let repeat = status.querySelector(".statbtn .repeat");
        
        let svg;
        if (type.value === "like" || type.value === "unlike")
            svg = [ like ];
        else if (type.value === "repeat" || type.value === "unrepeat")
            svg = [ repeat ];
        else if (type.value === "likeboost")
            svg = [ like, repeat ];
        
        svg.forEach(that => {
            let label = that.parentNode;
            let counter = label.querySelector(".count");
            
            let is_active = that.classList.contains("active");
            
            that.classList.toggle("active");


            if (is_active)
            {
                // Animation
                that.classList.remove("active-anim");
                
                // Flip itype value
                type.value = type.value.replace("un", "");
            }
            else {
                that.classList.add("active-anim");
                type.value = "un" + type.value;
            }
            
            counter.innerHTML = change_count_text(counter.innerHTML, is_active ? -1 : 1);
        });
    }

    function status_interact_props(e)
    {
        let interact = e.target.closest(".statbtn");
        let type = interact.parentNode.querySelector(".itype");
        if (type === null)
            return true;
        let status = interact.closest(".status");

        send_request("/treebird_api/v1/interact",
                     {
                         id: status.id,
                         itype: type.value
                     },
                     1,
                     (xhr, args) => {
                         if (xhr.status !== 200)
                         {
                             // Undo action if failure
                             interact_action(status, type);
                         }
                     }, null);

        interact_action(status, type);
        
        e.preventDefault();
        return false;
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
            let form = construct_quick_reply_form(status);
            form.insertAfter(status);
        }
        
        
        return false;
    }

    function frame_resize()
    {
        let rightbar_frame = document.querySelector("#rightbar .sidebar-frame");
        let rbar_frame_win = rightbar_frame.contentWindow;
        rightbar_frame.height = rbar_frame_win.document.body.scrollHeight;
    }

    function filesize_to_str(bs)
    {
        const val = bs === 0 ? 0 : Math.floor(Math.log(bs) / Math.log(1024));
        return (bs / 1024**val).toFixed(1) * 1 + ['B', 'kB', 'MB', 'GB', 'TB'][val];
    }

    function html_encode(str)
    {
        let en = document.createElement("span");
        en.textContent = str;
        return en.innerHTML;
    }

    function construct_file_upload(id, file, file_content)
    {
        let container = document.createElement("div");
        let content = document.createElement("img");
        let info = document.createElement("span");
        container.className = "file-upload";
        container.id = "file-id-" + id;
        
        info.className = "upload-info";
        info.innerHTML = `<span class="filesize">${filesize_to_str(file.size)}</span> &bull; <span class="filename">${html_encode(file.name)}</span>`;
        
        content.src = file_content;
        content.className = "upload-content";
        
        container.appendChild(content);
        container.appendChild(info);
        return container;
    }

    // Created if not exist
    function filepicker_create(id)
    {
        if (!state.file[id])
        {
            state.file[id] = { files: [], count: 0 }
            return true;
        }
        return false;
    }

    function filepicker_add(id, file)
    {
        filepicker_create(id);

        state.file[id].files.push(file);
        
        return state.file[id].count++;
    }

    function evt_file_upload(e)
    {
        let target = e.target;
        // is New?
        let id = state.file.file_counter;

        // TODO do something with this
        if (!target.classList.contains("used"))
        {
            target.id = `file-picker-id-${state.file.file_counter++}`;
            target.classList.add("used");
        }
        else {
            // "id-476" -> 476
            id = Number(target.id.substr("file-picker-id-".length));
        }

            
        let file_upload_dom = this.closest("form").querySelector(".file-uploads-container");
        file_upload_dom.className = "file-uploads-container";
        const files = [...this.files];

        let reader;

        // Create file upload
        for (let file of files)
        {
            reader = new FileReader();
            reader.onload = (() => {
                return (e) => {
                    let file_id = filepicker_add(id, file);
                    
                    file_upload_dom.appendChild(construct_file_upload(file_id, file, e.target.result));
                }
            })(file);
            reader.readAsDataURL(file);

        }
    }

    // Main (when loaded)
    document.addEventListener('DOMContentLoaded', () => {
        let reply_btn = document.getElementsByClassName("reply-btn");
        let interact_btn = document.getElementsByClassName("statbtn");

        // Add event listener to add specificied buttons
        for (let i = 0; i < reply_btn.length; ++i)
        {
            reply_btn[i].addEventListener('click', create_reply_form);
        }
        for (let i = 0; i < interact_btn.length; ++i)
        {
            interact_btn[i].addEventListener('click', status_interact_props);
        }
        
        // Resize notifications iFrame to full height
        let rightbar_frame = document.querySelector("#rightbar .sidebar-frame");
        if (rightbar_frame)
        {
            rightbar_frame.contentWindow.addEventListener('DOMContentLoaded', frame_resize);
        }

        // File upload
        let file_input = document.querySelector("input[type=file]");
        if (file_input)
        {
            file_input.addEventListener('change', evt_file_upload);
        }
    });
})();

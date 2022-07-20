(function(){
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
        xhr.open(type, url);
        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        xhr.onreadystatechange = function() {
            if (this.readyState === XMLHttpRequest.DONE)
                cb(this, cb_args);
        };
        xhr.send(query_str);
        return xhr;
    }

    function upload_file(url, file_name, file, cb, cb_args, onprogress, onload)
    {
        let xhr = new XMLHttpRequest();
        let form_data = new FormData();
        xhr.open("post", url);
        form_data.append(file_name, file);
        // xhr.setRequestHeader("Content-Type", "multipart/form-data");
        xhr.onreadystatechange = function() {
            if (this.readyState === XMLHttpRequest.DONE)
                cb(this, cb_args);
        };
        xhr.upload.onprogress = onprogress;
        xhr.upload.onload = onload;
        xhr.send(form_data);
        return xhr;
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
                     "POST",
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

    function construct_file_upload(file, file_content)
    {
        let container = document.createElement("div");
        let content = document.createElement("img");
        let info = document.createElement("span");
        container.className = "file-upload";
        
        info.className = "upload-info";
        info.innerHTML = `<span class="filesize">${filesize_to_str(file.size)}</span> &bull; <span class="filename">${html_encode(file.name)}</span>`;
        let progress_div = document.createElement("div");
        progress_div.className = "file-progress";
        info.appendChild(progress_div);
        
        content.src = file_content;
        content.className = "upload-content";
        
        container.appendChild(content);
        container.appendChild(info);
        return container;
    }

    function update_uploads_json(dom)
    {
        let root = dom.parentNode;
        let items = root.getElementsByClassName("file-upload");
        let ids = [];

        for (let i of items)
        {
            if (i.dataset.id)
                ids.push(i.dataset.id);
        }

        // Goto statusbox
        root = root.parentNode;
        let file_ids = root.querySelector(".file-ids-json");
        if (!file_ids)
        {
            // Create if doesn't exist
            file_ids = document.createElement("input");
            file_ids.type = "hidden";
            file_ids.className = "file-ids-json";
            file_ids.name = "fileids";
            root.appendChild(file_ids);
        }

        file_ids.value = JSON.stringify(ids);
    }

    function evt_file_upload(e)
    {
        let target = e.target;
        let file_upload_dom = this.closest("form").querySelector(".file-uploads-container");
        file_upload_dom.className = "file-uploads-container";
        const files = [...this.files];

        let reader;

        // Clear file input
        this.value = '';

        // Create file upload
        for (let file of files)
        {
            reader = new FileReader();
            reader.onload = (() => {
                return (e) => {
                    let file_dom = construct_file_upload(file, e.target.result);
                    
                    file_upload_dom.appendChild(file_dom);

                    let xhr = upload_file("/treebird_api/v1/attachment",
                                          "file",
                                          file,
                                          (xhr, args) => {
                                              // TODO errors
                                              file_dom.dataset.id = JSON.parse(xhr.response).id;
                                              update_uploads_json(file_dom);
                                          }, null,
                                          (e) => {
                                              let upload_file_progress = file_dom
                                                  .querySelector(".file-progress");
                                              // Add offset of 3
                                              upload_file_progress.style.width = 3+((e.loaded/e.total)*97);
                                          },
                                          (e) => {
                                              file_dom.querySelector(".upload-content").style.opacity = "1.0";
                                              file_dom.querySelector(".file-progress").remove();
                                          });
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
        let file_inputs = document.querySelectorAll(".statusbox input[type=file]");
        for (let file_input of file_inputs)
        {
            file_input.addEventListener('change', evt_file_upload);
        }
    });
})();

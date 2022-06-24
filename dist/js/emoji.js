let emojo_json = null;

// Get emojis from file
function get_emojo_json(callback)
{
    let xhr = new XMLHttpRequest();
    xhr.open("GET", "/emoji.json");
    xhr.onreadystatechange = function() {
        if (this.readyState === XMLHttpRequest.DONE)
            callback(this.response);
    };
    xhr.send();
}

function construct_emojo_picker(e)
{
    let index = 0;
    let emoji_picker = document.createElement("div");
    emoji_picker.className = "emoji-picker";
    emoji_picker.innerHTML = `<table class="tabs ui-table">
    <tr>
      <td>
        <label for="cat-smileys">
          <span class="tab-btn btn btn-alt">😃</span>
        </label>
      </td>
      <td>
        <label for="cat-animals">
          <span class="tab-btn btn btn-alt">🐻</span>
        </label>
      </td>
      <td>
        <label for="cat-food">
          <span class="tab-btn btn btn-alt">🍔</span>
        </label>
      </td>
      <td>
        <label for="cat-travel">
          <span class="tab-btn btn btn-alt">🚀</span>
        </label>
      </td>
      <td>
        <label for="cat-activities">
          <span class="tab-btn btn btn-alt">⚽</span>
        </label>
      </td>
      <td>
        <label for="cat-objects">
          <span class="tab-btn btn btn-alt">🔧</span>
        </label>
      </td>
      <td>
        <label for="cat-symbols">
          <span class="tab-btn btn btn-alt">🔢</span>
        </label>
      </td>
      <td>
        <label for="cat-flags">
          <span class="tab-btn btn btn-alt">🎌</span>
        </label>
      </td>
    </tr>
  </table>

  <div class="emoji-picker-emojos-wrapper">
    <input type="radio" class="hidden" id="cat-smileys" name="emoji-cat" checked>
    ${construct_emojis("Smileys & Emotion").outerHTML}
    <input type="radio" class="hidden" id="cat-animals" name="emoji-cat">
    ${construct_emojis("Animals & Nature", index).outerHTML}
    <input type="radio" class="hidden" id="cat-food" name="emoji-cat">
    ${construct_emojis("Food & Drink", index).outerHTML}
    <input type="radio" class="hidden" id="cat-travel" name="emoji-cat">
    ${construct_emojis("Travel & Places", index).outerHTML}
    <input type="radio" class="hidden" id="cat-activities" name="emoji-cat">
    ${construct_emojis("Activities", index).outerHTML}
    <input type="radio" class="hidden" id="cat-objects" name="emoji-cat">
    ${construct_emojis("Objects", index).outerHTML}
    <input type="radio" class="hidden" id="cat-symbols" name="emoji-cat">
    ${construct_emojis("Symbols", index).outerHTML}
    <input type="radio" class="hidden" id="cat-flags" name="emoji-cat">
    ${construct_emojis("Flags", index).outerHTML}
  </div>
</div>
`;

    return emoji_picker;
}

function construct_emojis(category, start_index = 0)
{
    let emoji_picker_div = document.createElement("div");
    emoji_picker_div.className = "emoji-picker-emojos";

    for (let i = 0; i < emojo_json.length; ++i)
    {
        if (emojo_json[i].group === category)
            emoji_picker_div.innerHTML += `<span class="emoji">${emojo_json[i].char}</span>`;
    }

    return emoji_picker_div;
}

get_emojo_json((emojo_json_str) => {
    emojo_json = JSON.parse(emojo_json_str);
    document.body.appendChild(construct_emojo_picker(emojo_json));
});

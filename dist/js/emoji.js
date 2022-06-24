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

function construct_emojo_picker()
{
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
    <div class="emoji-picker-emojos">
      {{%s:emojis_smileys}}
    </div>
    <input type="radio" class="hidden" id="cat-animals" name="emoji-cat">
    <div class="emoji-picker-emojos">
      {{%s:emojis_animals}}
    </div>
    <input type="radio" class="hidden" id="cat-food" name="emoji-cat">
    <div class="emoji-picker-emojos">
      {{%s:emojis_food}}
    </div>
    <input type="radio" class="hidden" id="cat-travel" name="emoji-cat">
    <div class="emoji-picker-emojos">
      {{%s:emojis_travel}}
    </div>
    <input type="radio" class="hidden" id="cat-activities" name="emoji-cat">
    <div class="emoji-picker-emojos">
      {{%s:emojis_activities}}
    </div>
    <input type="radio" class="hidden" id="cat-objects" name="emoji-cat">
    <div class="emoji-picker-emojos">
      {{%s:emojis_objects}}
    </div>
    <input type="radio" class="hidden" id="cat-symbols" name="emoji-cat">
    <div class="emoji-picker-emojos">
      {{%s:emojis_symbols}}
    </div>
    <input type="radio" class="hidden" id="cat-flags" name="emoji-cat">
    <div class="emoji-picker-emojos">
      {{%s:emojis_flags}}
    </div>
  </div>
</div>
`;

    return emoji_picker;
}

get_emojo_json((emojo_json_str) => {
    const emojo_json = JSON.parse(emojo_json_str);
    console.log(emojo_json);
});

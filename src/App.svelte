<script>
    const md = require('markdown-it')();

    let source = `### This is a test!
My paragraph in \`main.cpp\`.`;
</script>

<style>
  #container {
    display: flex;
    align-items: center;
  }
  .side {
    width: 50%;
    min-height: 100vh;
    padding: 0.5em;
    border: 1px solid red;
  }
  .side textarea {
    width: 100%;
    height: 100%;
  }
</style>
<div id="container">
  <!-- Left Side : source code -->
  <div class="side">
    <textarea bind:value={source}></textarea>
  </div>

  <!-- Right Side: Rendered code -->
  <div class="side">
    {@html md.render(source)}
  </div>
</div>

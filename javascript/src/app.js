import '../node_modules/katex/dist/katex.min.css';
import '../node_modules/highlight.js/styles/github.css';

import Tab from './Tab.svelte';

const app = new Tab({
  target: document.body,
  props: {
    source: '### Markdown title'
  }
});
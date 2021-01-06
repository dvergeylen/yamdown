import App from './App.svelte';

Neutralino.init({
  load: () => {
    const app = new App({
      target: document.body,
      props: {
        Neutralino,
        NL_ARGS,
      }
    });

    window.app = app;
  },
  pingSuccessCallback : function() {

  },
  pingFailCallback : function() {
    app.exit();
  }
});

import testNapi from 'librender.so';

@Component
export struct RenderView {
  private xComponentContext: object | undefined = undefined;

  build() {
    Column() {
      XComponent({ id: 'render_id', type: 'surface', libraryname: 'render' })
        .onLoad((context) => {
          this.xComponentContext = context;
          console.info('Load XComponent');
        })
        .onDestroy(() => {
          console.info('Destroy XComponent');
        })
        .width('100%')
        .height('100%')
        .onClick(() => {
          if (this.xComponentContext) {
            console.info('Click XComponent');
            testNapi.togglePause();
          }
        })
    }
  }
}

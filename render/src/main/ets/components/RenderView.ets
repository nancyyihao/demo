

interface RenderContext {
  togglePause(): void;
}

@Component
export struct RenderView {
  @Require @Prop renderId: string;
  private xComponentContext: RenderContext | undefined = undefined;

  build() {
    Column() {
      XComponent({ id: this.renderId, type: 'surface', libraryname: 'render' })
        .onLoad((context) => {
          this.xComponentContext = context as RenderContext;
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
            this.xComponentContext.togglePause();
          }
        })
    }
  }
}

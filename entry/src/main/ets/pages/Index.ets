import { RenderView } from 'render';

@Entry
@Component
struct Index {
  build() {
    Column() {
      // Top: Sine Wave (default or explicit 'sin')
      RenderView({ renderId: 'render_view_sin' })
        .width('100%')
        .height('50%')
      
      Divider().color(Color.Black).strokeWidth(2)
      
      // Bottom: Cosine Wave (id contains 'cos')
      RenderView({ renderId: 'render_view_cos' })
        .width('100%')
        .height('50%')
    }
    .height('100%')
    .width('100%')
  }
}
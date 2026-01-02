import { RenderView } from 'render';

@Entry
@Component
struct Index {
  build() {
    Column() {
      RenderView({ renderId: 'render_view_1' })
        .width('100%')
        .height('50%')
      
      // Divider for visual separation
      Divider().color(Color.Black).strokeWidth(2)
      
      RenderView({ renderId: 'render_view_2' })
        .width('100%')
        .height('50%')
    }
    .height('100%')
    .width('100%')
  }
}
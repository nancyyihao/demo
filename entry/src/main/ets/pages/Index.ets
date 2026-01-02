import { RenderView } from 'render';

@Entry
@Component
struct Index {
  build() {
    RelativeContainer() {
      RenderView()
        .alignRules({
          center: { anchor: '__container__', align: VerticalAlign.Center },
          middle: { anchor: '__container__', align: HorizontalAlign.Center }
        })
    }
    .height('100%')
    .width('100%')
  }
}
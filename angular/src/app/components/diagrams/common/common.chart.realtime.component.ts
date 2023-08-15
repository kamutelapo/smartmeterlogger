import { FactoryTarget } from '@angular/compiler';
import { CommonChartBaseComponent } from '../common/common.chart.base.component';

export class CommonChartRealtimeComponent extends CommonChartBaseComponent {
    xAxisLabel = 'Idő';
    interestedDate = new Date();
    hasError = false;

    timeFormat(date: Date) {
        if(date.getSeconds() == 0) {
          const ds = date.toLocaleTimeString("hu-HU")
          return ds.substr(0, ds.length-3)
        } else {
          return date.toLocaleTimeString("hu-HU")
        }
    }
}

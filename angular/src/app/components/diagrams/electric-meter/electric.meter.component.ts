import { Component, OnInit, ViewChild } from '@angular/core';
import { CommonChartRealtimeComponent } from '../common/common.chart.realtime.component';
import { Color, ScaleType } from '@swimlane/ngx-charts';
import { DataDownloadService, ParserConfig } from '../../../services/data.download.service';
import { ViewBoxCalculatorService } from '../../../services/viewbox.calculator.service';
import { CustomAreaChartStackedComponent } from '../../custom-charts/custom-stacked-area/custom.stacked.area.chart.component';
import { Tooltip } from '../common/common.chart.base.component';

@Component({
  selector: 'app-electric-meter',
  templateUrl: './electric.meter.component.html',
  styleUrls: ['./electric.meter.component.css']
})
export class ElectricMeterComponent extends CommonChartRealtimeComponent implements OnInit {
  title = "Villanyóra"

  yAxisLabel = 'Villanyóra';

  @ViewChild('chart') chart: CustomAreaChartStackedComponent | null = null;

  colorScheme: Color = {
    name: 'electricMeterScheme',
    selectable: true,
    group: ScaleType.Ordinal,
    domain: ['green', 'royalblue'],
  };

  parserConfig: ParserConfig = {
    interested: new Set<string>(["Time", "Export", "Import", "TotalExport", "TotalImport"]),
    dates: new Set<string>(["Time"]),
    numbers: new Set<string>(["Export", "Import", "TotalExport", "TotalImport"]),
  };

  totalConsumption = "0";
  totalBackFeed = "0";

  public constructor(private dataDownloadService: DataDownloadService, viewBoxCalculatorService: ViewBoxCalculatorService) {
    super(viewBoxCalculatorService);
    this.measurementUnit = "W";
  }

  onDateChange(): void {
    this.dataDownloadService.loadElectricMeterCSV(this.interestedDate).subscribe(
      data => {
          let map = this.dataDownloadService.parseCSV(data, this.parserConfig)

          if(map) {
            map['Export'] = map['Export'].map(function(x) { return x * -1000; });
            map['Import'] = map['Import'].map(function(x) { return x * 1000; });

            this.totalBackFeed = (map["TotalExport"][map["TotalExport"].length-1] - map["TotalExport"][0]).toFixed(1)
            this.totalConsumption = (map["TotalImport"][map["TotalImport"].length-1] - map["TotalImport"][0]).toFixed(1)
            this.multi = this.dataDownloadService.toSeries(map, 150, "Time", ["Import", "Export"],
               ["Fogyasztás (" + this.totalConsumption + " kWh)",
                "Visszatáplált (" + this.totalBackFeed + " kWh)", ], 0);
            this.hasError = false;

            if(this.chart) {
              this.chart.filteredDomain = null;
              this.chart.update();
            }
          } else {
            this.hasError = true;
          }
      },
      error => {
          this.multi = null;
          this.hasError = true;

          console.log(error);
      }
    );
  }

  override ngOnInit(): void {
    super.ngOnInit();
    this.onDateChange();
  }

  valueFormat(value: any) {
    return value + "W"
  }

  getModelName(value: string) {
    return value.replace(/ .*$/,"");
  }

  override getToolTipText(tooltipItem: Tooltip): string {
    let result = '';
    if (tooltipItem.series !== undefined) {
      result += tooltipItem.series.replace(/ .*$/,"");
    } else {
      result += '???';
    }
    result += ': ';
    if (tooltipItem.value !== undefined) {
      result += Math.abs(tooltipItem.value).toLocaleString("hu-HU") + this.measurementUnit;
    }
    return result;
  }
}

import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { HttpClientModule } from '@angular/common/http';
import { NgxChartsModule } from '@swimlane/ngx-charts';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { ElectricMeterComponent } from './components/diagrams/electric-meter/electric.meter.component';
import { CustomAreaChartStackedComponent } from './components/custom-charts/custom-stacked-area/custom.stacked.area.chart.component';


@NgModule({
  declarations: [
    AppComponent,
    CustomAreaChartStackedComponent,
    ElectricMeterComponent
  ],
  imports: [
    NgxChartsModule,
    BrowserModule,
    AppRoutingModule,
    HttpClientModule,
    BrowserAnimationsModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }

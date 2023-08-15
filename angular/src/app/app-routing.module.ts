import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { ElectricMeterComponent } from './components/diagrams/electric-meter/electric.meter.component';

const routes: Routes = [
  {
    path: 'electricmeter',
    component: ElectricMeterComponent
  },
  {
    path: '',
    redirectTo: 'electricmeter',
    pathMatch: 'full'
  }
];

@NgModule({
  imports: [RouterModule.forRoot(routes, {useHash : true})],
  exports: [RouterModule]
})
export class AppRoutingModule { }

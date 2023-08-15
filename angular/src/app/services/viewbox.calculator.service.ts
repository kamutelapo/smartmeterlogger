import { Injectable } from '@angular/core';

const HEADER_SIZE = 80;
const SIDENAV_SIZE = 210;

@Injectable({
    providedIn: 'root'
})
export class ViewBoxCalculatorService {
    
    private hasSideBar = true;

    private wide = true;

    private viewBox: [number, number] = [400, 250];

    public calculatViewBox(): void {
        const ratio = window.innerWidth / window.innerHeight;

        let deltaY = 0;
    
        if (ratio <= 1.66) {
          this.wide = false;
          deltaY = 50;
        } else {
          this.wide = true;
        }
        let width = window.innerWidth;
        if (this.hasSideBar) {
            width -= SIDENAV_SIZE;
        }
        if (width < 100) {
          width = 100;
        }
        let height = window.innerHeight - HEADER_SIZE - deltaY;
        if (height < 100) {
          height = 100;
        }

        this.viewBox = [width, height];
    }

    public isWideWindow(): boolean {
        return this.wide;
    }

    public getViewBox(): [number, number] {
        return this.viewBox;
    }

    public enableSideBar(en: boolean): void {
        this.hasSideBar = en;
    }

    public isMobile(): boolean {
      return window.innerWidth < 768;
    }
}

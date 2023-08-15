import { Injectable, Inject, LOCALE_ID, ɵɵsetComponentScope } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { formatDate } from "@angular/common";
import { Observable } from 'rxjs';

export class ParserConfig {
    interested: Set<string> = new Set();
    dates?: Set<string>;
    numbers?: Set<string>;
}

export type CsvDataType = {
    [id: string]: any[];
}

@Injectable({
    providedIn: 'root'
})
export class DataDownloadService {

    constructor(@Inject(LOCALE_ID) public locale: string, private http: HttpClient) {}

    public loadElectricMeterCSV(date: Date): Observable<string> {
        let fileName = formatDate(date, 'yyyy-MM-dd', this.locale) + ".csv"
        return this.http.get('../rt/eon/' + fileName, {responseType: 'text'});
    }

    public parseCSV(data: string, parserConfig: ParserConfig) {
        let csvToRowArray = data.split("\n")

        if (csvToRowArray.length > 0) {

            let headers = csvToRowArray[0].trim().split(",")

            let map: CsvDataType = {}

            let validColumns: Set<number> = new Set();

            let i = 0;
            headers.forEach(hdr => {
                if (parserConfig.interested.has(hdr)) {
                    map[hdr] = []
                    validColumns.add(i);
                }
                i++;
            })

            for (let index = 1; index < csvToRowArray.length; index++) {
                const line = csvToRowArray[index].trim();
                if (line.length < 1) {
                    continue;
                }
                let row = line.split(",");

                validColumns.forEach( (col) => {
                    let columnName = headers[col];
                    let value: any = row[col]

                    if (parserConfig.dates && parserConfig.dates.has(columnName)) {
                        value = new Date(Date.parse(value))
                    }
                    if (parserConfig.numbers && parserConfig.numbers.has(columnName)) {
                        value = Number(value)
                    }

                    map[columnName].push(value)
                })
            }


            return map
        }

        return null
    }

    public toSeries(map: CsvDataType, roundTo: number, timeField: string, fields: string[], fieldNames: string[], digits=2) {
        const merged = new Map<number, number[][]>();
        const offset = roundTo / 2;
        const dg = 10 ** digits;

        let ndx = 0;
        map[timeField].forEach( (f) => {
            let roundTime = Math.floor((f.getTime() + offset) / roundTo / 1000) * roundTo * 1000;

            if (! merged.has(roundTime) ) {
                merged.set(roundTime, []);
            }
            const arr = merged.get(roundTime);

            let newValues: number[] = [];
            fields.forEach(fld => {
                newValues.push(map[fld][ndx]);
            });
            arr?.push(newValues);
            ndx++;
        });

        const keys = [ ... merged.keys()].sort()

        const output: any[] = []

        for(let series=0; series < fields.length; series++) {
            output.push({
                "name": fieldNames[series],
                "series": [],
            });
        }

        keys.forEach(
            (date) => {
                const content = merged.get(date);
                if( content ) {
                    const dateInst = new Date(date)
                    let newValues: number[] = [];

                    for(let index=0; index < fields.length; index++) {
                        let avg = 0;
                        content?.forEach(c => {
                            avg += c[index];
                        });
                        avg /= content.length;

                        output[index]["series"].push(
                            {
                                "name": dateInst,
                                "value": Math.round(avg*dg) / dg,
                            }
                        )
                    }

                }
            }
        );

        return output;
    }
}

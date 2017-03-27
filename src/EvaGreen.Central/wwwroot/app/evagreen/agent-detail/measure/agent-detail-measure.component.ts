import { Component, Input, OnInit, AfterViewInit, ViewChild } from '@angular/core';
import { Data } from '../../shared/data.service';
import { nvD3 } from 'ng2-nvd3';
declare let d3: any;

@Component({
    moduleId: module.id,
    selector: 'agent-detail-measure',
    templateUrl: 'agent-detail-measure.component.html',
    styleUrls: ['agent-detail-measure.component.css']
})
export class AgentDetailMeasureComponent {

    @Input()
    measures: Data[];

    @ViewChild(nvD3)
    nvD3: nvD3;

    options: any;
    data: any;

    ngOnInit() {
        this.options = {
            chart: {
                type: 'linePlusBarChart',
                height: 500,
                margin: {
                    top: 30,
                    right: 75,
                    bottom: 50,
                    left: 75
                },
                bars: {
                    forceY: [0]
                },
                bars2: {
                    forceY: [0]
                },
                color: ['cornflowerblue', 'darkred'],
                xAxis: {
                    axisLabel: "Date",
                    tickFormat: function (d: any) {
                        return d3.time.format('%d/%m/%Y %H:%M')(new Date(d));
                    }
                },
                x2Axis: {
                    tickFormat: function (d: any) {
                        return d3.time.format('%x')(new Date(d));
                    },
                    showMaxMin: false
                },
                y1Axis: {
                    axisLabel: "Humidité",
                    tickFormat: function (d: any) {
                        return d3.format(',f')(d) + "%";
                    },
                    //axisLabelDistance: 12
                },
                y2Axis: {
                    axisLabel: "Température",
                    tickFormat: function (d: any) {
                        return d3.format(',f')(d) + "°C";
                    }
                },
                y3Axis: {
                    tickFormat: function (d: any) {
                        return d3.format(',f')(d) + "%";
                    }
                },
                y4Axis: {
                    tickFormat: function (d: any) {
                        return d3.format(',f')(d) + "°C";
                    }
                }
            }
        };
        this.data = [
            {
                key: "Humidité",
                bar: true,
                values: this.measures.filter(m => m.description == "2").map(m => {
                    return {
                        x: m.creationDate.valueOf() * 1000,
                        y: parseInt(m.value)
                    };
                })
            },
            {
                key: "Température",
                values: this.measures.filter(m => m.description == "1").map(m => {
                    return {
                        x: m.creationDate.valueOf() * 1000,
                        y: parseInt(m.value)
                    };
                })
            }
        ];
    }

    ngAfterViewInit() {
        this.nvD3.chart.update();
    }
}
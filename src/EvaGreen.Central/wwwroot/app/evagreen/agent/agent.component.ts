import { Component, Input, OnInit } from '@angular/core';

import { DataService, Data } from '../shared/data.service';
import { UserService } from '../shared/user.service';
import { Agent } from '../shared/agent.service';

@Component({
    moduleId: module.id,
    selector: 'agent-detail',
    templateUrl: 'agent.component.html',
    styleUrls: ['agent.component.css']
})
export class AgentComponent {

    @Input()
    agent: Agent;

    loading: boolean;
    datas: Data[];

    constructor(private dataService: DataService) {
        this.loading = true;
        this.datas = [];
    }

    ngOnInit() {
        this.loadDatas();
    }

    loadDatas() {
        this.loading = true;
        this.dataService.all(this.agent.id)
            .subscribe(result => {
                this.datas = result.sort((a, b) => a.integrationDate.valueOf() - b.integrationDate.valueOf());
                this.loading = false;
            });
    }

    getLastImagePath(): String {
        for (var index = 0; index < this.datas.length; index++) {
            var data = this.datas[index];
            if (data.type == Data.DATA_IMAGE) {
                return data.value;
            }
        }
        return "";
    }

    getLastContact(): String {
        if (this.datas.length > 0) {
            var recentlyReceived = this.datas[0];
            return this.timeDifference(new Date(), new Date(recentlyReceived.integrationDate.valueOf() * 1000));
        }
        return "jamas";
    }

    timeDifference(current: Date, previous: Date): String {

        var msPerMinute = 60 * 1000;
        var msPerHour = msPerMinute * 60;
        var msPerDay = msPerHour * 24;
        var msPerMonth = msPerDay * 30;
        var msPerYear = msPerDay * 365;

        var elapsed = (current.valueOf() - previous.valueOf());

        if (elapsed < msPerMinute) {
            return "il y à " + Math.round(elapsed / 1000) + " secondes";
        }
        else if (elapsed < msPerHour) {
            return "il y à " + Math.round(elapsed / msPerMinute) + " minutes";
        }
        else if (elapsed < msPerDay) {
            return "il y à " + Math.round(elapsed / msPerHour) + " heures";
        }
        else if (elapsed < msPerMonth) {
            return "il y à " + Math.round(elapsed / msPerDay) + " jours";
        }
        else if (elapsed < msPerYear) {
            return "il y à " + Math.round(elapsed / msPerMonth) + " mois";
        }
        else {
            return "il y à " + Math.round(elapsed / msPerYear) + " ans";
        }
    }
}
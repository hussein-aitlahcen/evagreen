import { Injectable } from '@angular/core';
import { AuthHttp } from 'angular2-jwt';
import { Observable } from 'rxjs/Observable';

import { DataService, Data } from './data.service';

export class Agent {

    public id: Number;
    public description: String;
    public uploadInterval: Number;
    public snapshotInterval: Number;
    public widthResolution: Number;
    public heightResolution: Number;
    public initialContact: Boolean;
    public datas: Data[];
    public loading: boolean;

    constructor(json: any, private dataService: DataService) {
        this.id = json.id;
        this.description = json.description;
        this.uploadInterval = json.uploadInterval;
        this.snapshotInterval = json.snapshotInterval;
        this.widthResolution = json.widthResolution;
        this.heightResolution = json.heightResolution;
        this.initialContact = json.initialContact;

        this.loading = true;
        this.dataService.all(this.id)
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
            return this.getTimeDifference(new Date(), new Date(recentlyReceived.integrationDate.valueOf() * 1000));
        }
        return "jamais";
    }

    getTimeDifference(current: Date, previous: Date): String {

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

@Injectable()
export class AgentService {
    constructor(private authHttp: AuthHttp, private dataService: DataService) { }

    all(): Observable<Agent[]> {
        return this.authHttp.get("/api/agent")
            .map(res => res.json().map((js: any) => new Agent(js, this.dataService)));
    }

    find(id: Number): Observable<Agent> {
        return this.authHttp.get("/api/agent/" + id)
            .map(res => new Agent(res.json(), this.dataService));
    }

    update(agent: Agent): Observable<Number> {
        return this.authHttp.put("/api/agent/" + agent.id, agent)
            .map(res => res.json().state);
    }
}
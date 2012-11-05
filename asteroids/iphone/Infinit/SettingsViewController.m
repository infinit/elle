//
//  SettingsViewController.m
//  Infinitest
//
//  Created by Simon Peccaud on 5/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "SettingsViewController.h"

@implementation SettingsViewController

@synthesize oTableView;
@synthesize optionsList;

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    UITableViewCell *logOutCell = [oTableView dequeueReusableCellWithIdentifier:@"logOutCell"];
    if (logOutCell == nil) {
        logOutCell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"logOutCell"];
    }
    UIButton *sampleButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [sampleButton setFrame:CGRectMake(logOutCell.frame.origin.x, logOutCell.frame.origin.y, logOutCell.frame.size.width - 19, logOutCell.frame.size.height)];
    [sampleButton setTitle:@"Log Out" forState:UIControlStateNormal];
    [sampleButton setBackgroundImage:[[UIImage imageNamed:@"red-button.png"] stretchableImageWithLeftCapWidth:10.0 topCapHeight:0.0] forState:UIControlStateNormal];
    [sampleButton addTarget:self action:@selector(logOut:) forControlEvents:UIControlEventTouchUpInside];
    [logOutCell.contentView addSubview:sampleButton];

    UITableViewCell *testCell = [oTableView dequeueReusableCellWithIdentifier:@"testCell"];
    if (testCell == nil) {
        testCell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"testCell"];
    }

    NSArray *firstSection = [NSArray arrayWithObjects:logOutCell, nil];
    NSArray *secondSection = [NSArray arrayWithObjects:testCell,
                                [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"testCell"], nil];
//    NSArray *thirdSection = [NSArray arrayWithObject:@"Yellow"];

    optionsList = [[NSMutableArray alloc] initWithObjects:firstSection, secondSection/*, thirdSection*/, nil];

    [self.tableView setBackgroundView:nil];
    [self.tableView setBackgroundColor:[UIColor clearColor]];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return [[self optionsList] count];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [[[self optionsList] objectAtIndex:section] count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSArray *sectionContents = [[self optionsList] objectAtIndex:[indexPath section]];
    UITableViewCell *contentForThisRow = [sectionContents objectAtIndex:[indexPath row]];

    if (contentForThisRow) {        // Configure the cell.
        return contentForThisRow;
        //cell.textLabel.text = contentForThisRow;
    }
    else {
        static NSString *CellIdentifier = @"Cell";
        UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];

        if (cell == nil) {
            cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
        }

        return cell;
    }
}

/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/

/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:UITableViewRowAnimationFade];
    }
    else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }
}
*/

/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath
{
}
*/

/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Navigation logic may go here. Create and push another view controller.
    /*
     <#DetailViewController#> *detailViewController = [[<#DetailViewController#> alloc] initWithNibName:@"<#Nib name#>" bundle:nil];
     // ...
     // Pass the selected object to the new view controller.
     [self.navigationController pushViewController:detailViewController animated:YES];
     */
}

- (IBAction)logOut:(id)sender {
    [[NSNotificationCenter defaultCenter] postNotificationName:@"didLogOut" object:self];
}

@end

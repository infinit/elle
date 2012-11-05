//
//  DriveInfoViewController.m
//  Infinit
//
//  Created by Simon Peccaud on 9/4/12.
//  Copyright (c) 2012 infinit.io. All rights reserved.
//

#import "DriveInfoViewController.h"

@interface DriveInfoViewController ()

@end

@implementation DriveInfoViewController

@synthesize infoList;
@synthesize oTableView;
@synthesize ifNetwork;

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
    
    UITableViewCell *testCell = [self.oTableView dequeueReusableCellWithIdentifier:@"testCell"];
    if (testCell == nil) {
        testCell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"testCell"];
    }

    NSArray *firstSection = [NSArray arrayWithObjects: nil];
    NSArray *secondSection = [NSArray arrayWithObjects:testCell,
                              [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"testCell"], nil];
    
    self.infoList = [[NSMutableArray alloc] initWithObjects:firstSection, secondSection/*, thirdSection*/, nil];
    
    [self setTitle:ifNetwork.name];
}

- (DriveInfoViewController*) initWithInfinitNetwork:(InfinitNetwork*) ifNet {
    self = [super init];
    
    self.ifNetwork = ifNet;
    
    return self;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return [[self infoList] count];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [[[self infoList] objectAtIndex:section] count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSArray *sectionContents = [[self infoList] objectAtIndex:[indexPath section]];
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
        [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
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



@end

@implementation DriveCell

@synthesize infoButton;
@synthesize offlineIcon;

- (id) initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];

    self.infoButton = [UIButton buttonWithType:UIButtonTypeInfoDark];

    self.infoButton.backgroundColor = [UIColor clearColor];
    CGRect driveInfoButtonRect = CGRectMake(self.frame.size.width - self.infoButton.frame.size.width - 10,
                                            self.infoButton.frame.size.height,
                                            self.infoButton.frame.size.width,
                                            self.infoButton.frame.size.height);
    [self.infoButton setFrame:driveInfoButtonRect];
    [self.infoButton setAlpha:0.75];

    [self.contentView addSubview:self.infoButton];

    return self;
}

// Overriding these methods avoids unwanted infoBUtton highlight when pressing cell
- (void)setHighlighted:(BOOL)highlighted animated:(BOOL)animated {
    [self.infoButton setShowsTouchWhenHighlighted:FALSE];
    [super setHighlighted:highlighted animated:animated];
    self.infoButton.highlighted = NO;
    [self.infoButton setShowsTouchWhenHighlighted:TRUE];
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [self.infoButton setShowsTouchWhenHighlighted:FALSE];
    [super setSelected:selected animated:animated];
    self.infoButton.selected = NO;
    // If you don't set highlighted to NO in this method,
    // for some reason it'll be highlighed while the 
    // table cell selection animates out
    self.infoButton.highlighted = NO;
    [self.infoButton setShowsTouchWhenHighlighted:TRUE];
}

- (void) hideInfoButton {
    [self.infoButton removeFromSuperview];
}

- (void) revealInfoButton {
    [self.contentView addSubview:self.infoButton];
}

@end

